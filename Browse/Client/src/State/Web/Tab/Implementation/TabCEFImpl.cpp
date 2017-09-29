//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Mueller (muellerd@uni-koblenz.de)
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "src/State/Web/Tab/Tab.h"
#include "src/Utils/Logger.h"
#include "src/Setup.h"
#include "src/State/Web/Managers/HistoryManager.h"
#include "src/State/Web/Tab/Pipelines/JSDialogPipeline.h"
#include "src/Singletons/LabStreamMailer.h"
#include "src/CEF/Mediator.h"
#include <algorithm>

#define SendRenderMessage [this](CefRefPtr<CefProcessMessage> msg) { return _pCefMediator->SendProcessMessageToRenderer(msg, this); }

void Tab::GetWebRenderResolution(int& rWidth, int& rHeight) const
{
	// One cannot ask the web view for its size, because its size is not updated before the update call
	// of tab happens in the next frame. But this method is called by CEF directly at resize callback,
	// which means before the update call. Ideas for improvement are welcomed
	auto webViewInGUI = eyegui::getAbsolutePositionAndSizeOfElement(_pPanelLayout, "web_view");
	rWidth = webViewInGUI.width * setup::WEB_VIEW_RESOLUTION_SCALE;
	rHeight = webViewInGUI.height * setup::WEB_VIEW_RESOLUTION_SCALE;
}

void Tab::SetURL(std::string URL)
{
	if (URL != _url)
	{
		_url = URL;
		LabStreamMailer::instance().Send("Loading URL: " + _url);

		// Add history entry for new url with current title, will be updated asap
		HistoryManager::Page page;
		page.URL = _url;
		page.title = _title;
		_pWeb->PushAddPageToHistoryJob(this, page);
	}
}

void Tab::ReceiveFaviconBytes(std::unique_ptr< std::vector<unsigned char> > upData, int width, int height)
{
	// Go over colors and get most colorful as accent (should be always RGBA)
	int size; // width * height * 4
	if (upData != NULL && ((size = (int)upData->size()) >= 4) && width > 0 && height == width) // only accept square icons
	{
		if (size <= _current_favicon_bytes)
			return;

		LogInfo("Tab: Current favicon resolution -- ", width, " x ", height);
		_current_favicon_bytes = size;

		// Load icon into eyeGUI
		eyegui::fetchImage(_pPanelLayout, GetFaviconIdentifier(), width, height, eyegui::ColorFormat::RGBA, upData->data(), true);
		_faviconLoaded = true;

		// Prepare loop
		int steps = (width * height) / TAB_ACCENT_COLOR_SAMPLING_POINTS;
		steps = glm::max(1, steps);
		int maxIndex = 0;
		int maxSaturation = 0;
		for (int i = 0; i < size; i += steps * 4)
		{
			// Extract colors
			float r = upData->at(i);
			float g = upData->at(i + 1);float b = upData->at(i + 2);
			// float a = upData->at(i + 3); // not used

			// Calculate saturation like in HSV color space
			float max = glm::max(r, glm::max(g, b));
			float saturation = 0;
			if (max != 0)
			{
				float delta = max - glm::min(r, glm::min(g, b));
				saturation = delta / max;
				// saturation *= a; // Are values already premultiplied?
			}

			// Is it maximum?
			if (maxSaturation < saturation)
			{
				maxSaturation = saturation;
				maxIndex = i;
			}
		}

		// Extract accent color
		_targetColorAccent = glm::vec4(
			((float)upData->at(maxIndex) / 255.f),
			((float)upData->at(maxIndex + 1) / 255.f),
			((float)upData->at(maxIndex + 2) / 255.f),
			1.f);

		// Check, whether new target color is too much white
		float sum = _targetColorAccent.r + _targetColorAccent.g + _targetColorAccent.b; // maximal 3
		float whiteBorder = 2.5f;
		if (sum >= whiteBorder)
		{
			// Too bright, darken it
			float multiplier = (1.f - (sum - whiteBorder));
			_targetColorAccent.r *= multiplier;
			_targetColorAccent.g *= multiplier;
			_targetColorAccent.b *= multiplier;
		}
		else if (sum <= 0.3f)
		{
			// Too dark, use default instead
			_targetColorAccent = TAB_DEFAULT_COLOR_ACCENT;
		}

        // Start color accent interpolation
        _colorInterpolation = 0;

	}
    // else: do nothing
}

void Tab::ResetFaviconBytes()
{
	LogInfo("Tab: ResetFaviconBytes called!");
    _faviconLoaded = false;
	_current_favicon_bytes = 0;
	_loaded_favicon_urls.clear();
}

bool Tab::IsFaviconAlreadyAvailable(std::string img_url)
{
	if (std::find(_loaded_favicon_urls.begin(), _loaded_favicon_urls.end(), img_url) == _loaded_favicon_urls.end())
	{
		_loaded_favicon_urls.push_back(img_url);
		return false;
	}
	return true;
}

void Tab::AddDOMTextInput(CefRefPtr<CefBrowser> browser, int id)
{
	std::shared_ptr<DOMTextInput> spNode = std::make_shared<DOMTextInput>(
		id,
		SendRenderMessage);

	// Add node to ID->node map
	_TextInputMap.emplace(id, spNode);

	// Create DOMTrigger
	std::unique_ptr<TextInputTrigger> upDOMTrigger = std::unique_ptr<TextInputTrigger>(new TextInputTrigger(this, _triggers, spNode));

	// Activate trigger
	if (!_pipelineActive)
	{
		upDOMTrigger->Activate();
	}

	// Place trigger in map
	_textInputTriggers.emplace(id, std::move(upDOMTrigger));
}

void Tab::AddDOMLink(CefRefPtr<CefBrowser> browser, int id)
{
	_TextLinkMap.emplace(id, std::make_shared<DOMLink>(
		id,
		SendRenderMessage));
}

void Tab::AddDOMSelectField(CefRefPtr<CefBrowser> browser, int id)
{
	std::shared_ptr<DOMSelectField> spNode = std::make_shared<DOMSelectField>(
		id,
		SendRenderMessage);

	// Add node to ID->node map
	_SelectFieldMap.emplace(id, spNode);

	// Create DOMTrigger
	std::unique_ptr<SelectFieldTrigger> upDOMTrigger = std::unique_ptr<SelectFieldTrigger>(new SelectFieldTrigger(this, _triggers, spNode));

	// Activate trigger
	if (!_pipelineActive)
	{
		upDOMTrigger->Activate();
	}

	// Place trigger in map
	_selectFieldTriggers.emplace(id, std::move(upDOMTrigger));
}

void Tab::AddDOMOverflowElement(CefRefPtr<CefBrowser> browser, int id)
{
	_OverflowElementMap.emplace(id, std::make_shared<DOMOverflowElement>(
		id,
		SendRenderMessage));
}

void Tab::AddDOMVideo(CefRefPtr<CefBrowser> browser, int id)
{
	std::shared_ptr<DOMVideo> spNode = std::make_shared<DOMVideo>(
		id,
		SendRenderMessage);

	// Add node to ID->node map
	_VideoMap.emplace(id, spNode);

	// Create DOMTrigger
	std::unique_ptr<VideoModeTrigger> upDOMTrigger = std::unique_ptr<VideoModeTrigger>(new VideoModeTrigger(this, _triggers, spNode,
		[&](int id)
	{
		this->EnterVideoMode(id);
	}));

	// Activate trigger
	if (!_pipelineActive)
	{
		upDOMTrigger->Activate();
	}

	// Place trigger in map
	_videoModeTriggers.emplace(id, std::move(upDOMTrigger));
}

std::weak_ptr<DOMTextInput> Tab::GetDOMTextInput(int id)
{
	return (_TextInputMap.find(id) != _TextInputMap.end()) ? _TextInputMap.at(id) : std::weak_ptr<DOMTextInput>();
}

std::weak_ptr<DOMLink> Tab::GetDOMLink(int id)
{
	return (_TextLinkMap.find(id) != _TextLinkMap.end()) ? _TextLinkMap.at(id) : std::weak_ptr<DOMLink>();
}

std::weak_ptr<DOMSelectField> Tab::GetDOMSelectField(int id)
{
	return (_SelectFieldMap.find(id) != _SelectFieldMap.end()) ? _SelectFieldMap.at(id) : std::weak_ptr<DOMSelectField>();
}

std::weak_ptr<DOMOverflowElement> Tab::GetDOMOverflowElement(int id)
{
	return (_OverflowElementMap.find(id) != _OverflowElementMap.end()) ? _OverflowElementMap.at(id) : std::weak_ptr<DOMOverflowElement>();
}

std::weak_ptr<DOMVideo> Tab::GetDOMVideo(int id)
{
	return (_VideoMap.find(id) != _VideoMap.end()) ? _VideoMap.at(id) : std::weak_ptr<DOMVideo>();
}

void Tab::ClearDOMNodes()
{
	// Deactivate all triggers
	for (auto pTrigger : _triggers)
	{
		pTrigger->Deactivate();
	}

	// Clear vector with triggers
	_textInputTriggers.clear();
	_selectFieldTriggers.clear();
	_videoModeTriggers.clear();

	// Clear ID->node maps
	_TextLinkMap.clear();
	_TextInputMap.clear();
	_SelectFieldMap.clear();
	_VideoMap.clear();

	// Clear fixed elements
	_fixedElements.clear();

	// Clear overflow elements
	_OverflowElementMap.clear();
}

void Tab::RemoveDOMTextInput(int id)
{
	if (_textInputTriggers.find(id) != _textInputTriggers.end()) { _textInputTriggers.erase(id); }
	if (_TextInputMap.find(id) != _TextInputMap.end()) { _TextInputMap.erase(id); }
}

void Tab::RemoveDOMLink(int id)
{
	if (_TextLinkMap.find(id) != _TextLinkMap.end()) { _TextLinkMap.erase(id); }
}

void Tab::RemoveDOMSelectField(int id)
{
	if (_selectFieldTriggers.find(id) != _selectFieldTriggers.end()) { _selectFieldTriggers.erase(id); }
	if (_SelectFieldMap.find(id) != _SelectFieldMap.end()) { _SelectFieldMap.erase(id); }
}

void Tab::RemoveDOMOverflowElement(int id)
{
	if (_OverflowElementMap.find(id) != _OverflowElementMap.end()) { _OverflowElementMap.erase(id); }
}

void Tab::RemoveDOMVideo(int id)
{
	// Exit video mode if currently showing the video
	if (id == _videoModeId)
	{
		ExitVideoMode();
	}

	if (_videoModeTriggers.find(id) != _videoModeTriggers.end()) { _videoModeTriggers.erase(id); }
	if (_VideoMap.find(id) != _VideoMap.end()) { _VideoMap.erase(id); }
}

void Tab::SetScrollingOffset(double x, double y)
{
	// Update page size information when nearly reached end of page size (in case of endless scrollable sites like e.g. Fb.com)
	// (but don't do it if scrolling offset hasn't changed)
	/*
	auto webViewPositionAndSize = CalculateWebViewPositionAndSize();
	if ((x != _scrollingOffsetX && abs(_pageWidth - webViewPositionAndSize.width - x) < 50)
	|| (y != _scrollingOffsetY && abs(_pageHeight - webViewPositionAndSize.height - y) < 50))
	{
	_pCefMediator->GetPageResolution(this);
	}
	*/

	_scrollingOffsetX = x;
	_scrollingOffsetY = y;
}

void Tab::SetPageResolution(double width, double height)
{
	if (width != _pageWidth || height != _pageHeight)
	{
		LogInfo("Tab: Page size (w: ", _pageWidth, ", h: ", _pageHeight, ") changed to (w: ", width, ", h: ", height, ").");
	}
	_pageWidth = width;
	_pageHeight = height;
}

void Tab::AddFixedElementsCoordinates(int id, std::vector<Rect> elements)
{
	// Assign list of fixed element coordinates to given position
	if ((int)_fixedElements.size() <= id)
	{
		_fixedElements.resize(id + 1);
	}
	_fixedElements[id] = elements;

	 //DEBUG
	//LogDebug("------------------ TAB ------------------> BEGIN");
	//LogDebug("Added fixed element with id=", id);
	//LogDebug("#fixedElements: ", _fixedElements.size());
	//for (int i = 0; i < _fixedElements.size(); i++)
	//{
	//	if (_fixedElements[i].empty())
	//	{
	//		LogDebug(i, ": {}");
	//	}
	//	else if (_fixedElements[i][0].isZero())
	//	{
	//		LogDebug(i, ": 0");
	//	}
	//	else
	//	{
	//		for (const auto& rect : _fixedElements[i])
	//		{
	//			LogDebug(i, ": ", rect.toString());
	//		}
	//	}
	//}
	//LogDebug("------------------ TAB ------------------< END");
}

void Tab::RemoveFixedElement(int id)
{
	// DEBUG
	//LogDebug("Tab: Remove fixed element id=", id);

	if ((int)_fixedElements.size() > id)
	{
		_fixedElements[id].clear();
	}
	//else
	//{
	//	LogDebug("Tab: Fixed element with id=", id, " wasn't in list...");
	//}
}

void Tab::SetTitle(std::string title)
{
	if (title != _title)
	{
		LogDebug("Tab: Set title to ", title, " - previous title=", _title);

		_title = title;

		auto last_entry = _pWeb->GetLastHistoryEntry();

		LogDebug("Tab: last entry url=", last_entry.URL, " title=", last_entry.title, " -- now: title=", title, " url=", _url);

		if (last_entry.URL == _url && last_entry.title != title)
		{
			HistoryManager::Page page;
			page.title = _title;
			page.URL = _url;

			// Delete last entry with identical URL from history (also XML)
			_pWeb->PushDeletePageFromHistoryJob(this, page, true);

			// Add updated entry to history
			_pWeb->PushAddPageToHistoryJob(this, page);
		}
	}
}

void Tab::SetLoadingStatus(bool isLoading)
{
	// isLoading=false may indicate, that site has completely finished loading
	if(isLoading)
    {
        // Main frame is loading
        eyegui::setImageOfPicture(_pPanelLayout, "icon", "icons/TabLoading_0.png");
        _timeUntilNextLoadingIconFrame = TAB_LOADING_ICON_FRAME_DURATION;
        _iconState = IconState::LOADING;

		// Abort any pipeline execution when loading of main frame starts
		AbortAndClearPipelines();
    }
    else
    {
        // Main frame is done with loading
        if (_faviconLoaded)
        {
            eyegui::setImageOfPicture(_pPanelLayout, "icon", GetFaviconIdentifier());
            _iconState = IconState::FAVICON;
        }
        else
        {
            eyegui::setImageOfPicture(_pPanelLayout, "icon", "icons/TabIconNotFound.png");
            _iconState = IconState::ICON_NOT_FOUND;
        }

		//// Add page to history after loading
		//if (_dataTransfer)
		//{
		//	HistoryManager::Page page;
		//	page.URL = _url;
		//	page.title = _title;
		//	_pWeb->PushAddPageToHistoryJob(this, page);
		//}
    }
}

void Tab::RequestJSDialog(JavaScriptDialogType type, std::string message)
{
	this->PushBackPipeline(std::unique_ptr<Pipeline>(new JSDialogPipeline(this, type, message)));
}