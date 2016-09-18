//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Mueller (muellerd@uni-koblenz.de)
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "src/State/Web/Tab/Tab.h"
#include "src/Utils/Logger.h"
#include <algorithm>

void Tab::GetWebRenderResolution(int& rWidth, int& rHeight) const
{
	auto positionAndSize = CalculateWebViewPositionAndSize();
	rWidth = positionAndSize.width;
	rHeight = positionAndSize.height;
}

void Tab::ReceiveFaviconBytes(std::unique_ptr< std::vector<unsigned char> > upData, int width, int height)
{
	// Go over colors and get most colorful as accent (should be always RGBA)
	int size; // width * height * 4
	if (upData != NULL && ((size = (int)upData->size()) >= 4) && width > 0 && height > 0)
	{
        // Create name of image by using tab information for having a unique image for each tab
        int tabId = _pWeb->GetIdOfTab(this);
        if(tabId >= 0)
        {
            std::string imageName = "tab_info_" + std::to_string(tabId);

            // Show favicon in layout TODO: save? favicon until page loaded
            // eyegui::setImageOfPicture(_pPanelLayout, "icon", imageName, width, height, eyegui::ColorFormat::RGBA, upData->data(), true);
        }

		// Prepare loop
		int steps = (width * height) / TAB_ACCENT_COLOR_SAMPLING_POINTS;
		steps = glm::max(1, steps);
		int maxIndex = 0;
		int maxSaturation = 0;
		for (int i = 0; i < size; i += steps * 4)
		{
			// Extract colors
			float r = upData->at(i);
			float g = upData->at(i + 1);
			float b = upData->at(i + 2);
			float a = upData->at(i + 3);

			// Calculate saturation like in HSV color space
			float max = glm::max(r, glm::max(g, b));
			float saturation = 0;
			if (max != 0)
			{
				float delta = max - glm::min(r, glm::min(g, b));
				saturation = delta / max;
				saturation *= a;
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
	}
	else
	{
		// Something went wrong at favicon loading
		_targetColorAccent = TAB_DEFAULT_COLOR_ACCENT;
	}

	// Start color accent interpolation
	_colorInterpolation = 0;
}

void Tab::ResetFaviconBytes()
{
	// TODO
}

void Tab::AddDOMNode(std::shared_ptr<DOMNode> spNode)
{
	//LogDebug("Tab: Adding new DOM node with id=", spNode->GetNodeID(), " & type=", spNode->GetType());

	// Decide what to do with node
	switch (spNode->GetType())
	{
	case DOMNodeType::TextInput:
	{
		// Add node to ID->node map
		_TextInputMap.emplace(spNode->GetNodeID(), spNode);

		// Create DOMTrigger
		std::unique_ptr<DOMTrigger> upDOMTrigger = std::unique_ptr<DOMTrigger>(new DOMTrigger(this, spNode));

		// Activate trigger
		if (!_pipelineActive)
		{
			upDOMTrigger->Activate();
		}

		// Push it to vector
		_DOMTriggers.push_back(std::move(upDOMTrigger));

		break;
	}

	case DOMNodeType::TextLink:
	{
		// Just add it to vector
		_DOMTextLinks.push_back(spNode);

		// Add node to ID->node map
		_TextLinkMap.emplace(spNode->GetNodeID(), spNode);

		break;
	}
	}
}

void Tab::ClearDOMNodes()
{
	// Deactivate all DOMTriggers
	for (auto& upDOMTrigger : _DOMTriggers)
	{
		upDOMTrigger->Deactivate();
	}

	// Clear vector with triggers
	_DOMTriggers.clear();

	// Clear vector with text links
	_DOMTextLinks.clear();

	// Clear ID->node maps
	_TextLinkMap.clear();
	_TextInputMap.clear();

	// Clear fixed elements
	_fixedElements.clear();
}

void Tab::RemoveDOMNode(DOMNodeType type, int nodeID)
{

	std::map<int, std::shared_ptr<DOMNode> >* map;
	std::vector<std::shared_ptr<DOMNode> >* list;

	switch (type)
	{
	case(DOMNodeType::TextInput) : { map = &_TextInputMap; break; }
	case(DOMNodeType::TextLink) : { map = &_TextLinkMap; list = &_DOMTextLinks; break; }
	}

	// Remove node's ID from map
	if (map->find(nodeID) != map->end())
	{
		map->erase(nodeID);
	}

	// TODO: List for all types of nodes?
	// TODO (maybe Raphael): Remove one specific DOMTrigger belonging to input field with id= nodeID
	if (list)
	{
		// Remove node from list of all nodes, if condition holds
		std::remove_if(
			list->begin(),
			list->end(),
			[nodeID](const std::shared_ptr<DOMNode>& node) {
			return node->GetNodeID() == nodeID;
		}
		);
	}
}

std::weak_ptr<DOMNode> Tab::GetDOMNode(DOMNodeType type, int nodeID)
{
	switch (type)
	{
		case DOMNodeType::TextInput: { 
			return (_TextInputMap.find(nodeID) != _TextInputMap.end()) ? _TextInputMap.at(nodeID) : std::weak_ptr<DOMNode>(); 
		}
		case DOMNodeType::TextLink: {
			return (_TextLinkMap.find(nodeID) != _TextLinkMap.end()) ? _TextLinkMap.at(nodeID) : std::weak_ptr<DOMNode>();
		}
	}
	return std::weak_ptr<DOMNode>();
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
	/*LogDebug("Added fixed element with id=", id);
	LogDebug("#fixedElements: ", _fixedElements.size());
	for (int i = 0; i < _fixedElements.size(); i++)
	{
		if (_fixedElements[i].empty())
		{
			LogDebug(i, ": {}");
		}
		else if (_fixedElements[i][0].isZero())
		{
			LogDebug(i, ": 0");
		}
		else 
			LogDebug(i, ": ", _fixedElements[i][0].toString());
	}*/
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

void Tab::SetLoadingStatus(int64 frameID, bool isMain, bool isLoading)
{
	// isMain=true && isLoading=false may indicate, that site has completely finished loading
	if (isMain)
	{
		if (isLoading)
		{
			SetIconState(IconState::LOADING);
		}
		else
		{
			SetIconState(IconState::FAVICON);
		}
	}
}
