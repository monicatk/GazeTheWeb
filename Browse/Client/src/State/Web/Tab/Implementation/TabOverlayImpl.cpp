//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Mueller (muellerd@uni-koblenz.de)
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "src/State/Web/Tab/Tab.h"
#include "src/Master.h"
#include "src/Utils/Logger.h"

int Tab::AddFloatingFrameToOverlay(
	std::string brickFilepath,
	float relativePositionX,
	float relativePositionY,
	float relativeSizeX,
	float relativeSizeY,
	std::map<std::string, std::string> idMapper)
{
	return eyegui::addFloatingFrameWithBrick(
		_pOverlayLayout,
		brickFilepath,
		relativePositionX,
		relativePositionY,
		relativeSizeX,
		relativeSizeY,
		idMapper,
		false,
		false);
}

void Tab::SetPositionOfFloatingFrameInOverlay(
	int index,
	float relativePositionX,
	float relativePositionY)
{
	eyegui::setPositionOfFloatingFrame(_pOverlayLayout, index, relativePositionX, relativePositionY);
}

void Tab::SetVisibilyOfFloatingFrameInOverlay(int index, bool visible)
{
	// Reset when becoming visible
	eyegui::setVisibilityOFloatingFrame(_pOverlayLayout, index, visible, visible, true);
}

void Tab::RemoveFloatingFrameFromOverlay(int index)
{
	eyegui::removeFloatingFrame(_pOverlayLayout, index, true);
}

void Tab::RegisterButtonListenerInOverlay(std::string id, std::function<void(void)> downCallback, std::function<void(void)> upCallback)
{
	// Log bug when id already existing
	auto iter = _overlayButtonDownCallbacks.find(id);
	if (iter != _overlayButtonDownCallbacks.end())
	{
		LogBug("Tab: Element id exists already in overlay button down callbacks: ", id);
	}
	iter = _overlayButtonUpCallbacks.find(id);
	if (iter != _overlayButtonUpCallbacks.end())
	{
		LogBug("Tab: Element id exists already in overlay button up callbacks: ", id);
	}

	_overlayButtonDownCallbacks.emplace(id, downCallback);
	_overlayButtonUpCallbacks.emplace(id, upCallback);

	// Tell eyeGUI about it
	eyegui::registerButtonListener(_pOverlayLayout, id, _spTabOverlayButtonListener);
}

void Tab::UnregisterButtonListenerInOverlay(std::string id)
{
	_overlayButtonDownCallbacks.erase(id);
	_overlayButtonUpCallbacks.erase(id);
}

void Tab::RegisterKeyboardListenerInOverlay(std::string id, std::function<void(std::u16string)> callback)
{
	// Log bug when id already existing
	auto iter = _overlayKeyboardCallbacks.find(id);
	if (iter != _overlayKeyboardCallbacks.end())
	{
		LogBug("Tab: Element id exists already in overlay keyboard callbacks: ", id);
	}

	_overlayKeyboardCallbacks.emplace(id, callback);

	// Tell eyeGUI about it
	eyegui::registerKeyboardListener(_pOverlayLayout, id, _spTabOverlayKeyboardListener);
}

void Tab::UnregisterKeyboardListenerInOverlay(std::string id)
{
	_overlayKeyboardCallbacks.erase(id);
}

void Tab::SetCaseOfKeyboardLetters(std::string id, bool upper)
{
	eyegui::setCaseOfKeyboard(_pOverlayLayout, id, upper ? eyegui::KeyboardCase::UPPER : eyegui::KeyboardCase::LOWER);
}

void Tab::RegisterWordSuggestListenerInOverlay(std::string id, std::function<void(std::u16string)> callback)
{
	// Log bug when id already existing
	auto iter = _overlayWordSuggestCallbacks.find(id);
	if (iter != _overlayWordSuggestCallbacks.end())
	{
		LogBug("Tab: Element id exists already in overlay word suggest callbacks: ", id);
	}

	_overlayWordSuggestCallbacks.emplace(id, callback);

	// Tell eyeGUI about it
	eyegui::registerWordSuggestListener(_pOverlayLayout, id, _spTabOverlayWordSuggestListener);
}

void Tab::UnregisterWordSuggestListenerInOverlay(std::string id)
{
	_overlayWordSuggestCallbacks.erase(id);
}

void Tab::DisplaySuggestionsInWordSuggest(std::string id, std::u16string input)
{
	if (input.empty())
	{
		eyegui::clearSuggestions(_pOverlayLayout, id);
	}
	else
	{
		eyegui::suggestWords(_pOverlayLayout, id, input, _pMaster->GetDictionary());
	}
}

void Tab::GetScrollingOffset(double& rScrollingOffsetX, double& rScrollingOffsetY) const
{
	rScrollingOffsetX = _scrollingOffsetX;
	rScrollingOffsetY = _scrollingOffsetY;
}

void Tab::SetContentOfTextBlock(std::string id, std::u16string content)
{
	eyegui::setContentOfTextBlock(_pOverlayLayout, id, content);
}

int Tab::GetWebViewX() const
{
	return _upWebView->GetX();
}

int Tab::GetWebViewY() const
{
	return _upWebView->GetY();
}

int Tab::GetWebViewWidth() const
{
	return _upWebView->GetWidth();
}

int Tab::GetWebViewHeight() const
{
	return _upWebView->GetHeight();
}

int Tab::GetWindowWidth() const
{
	return _pMaster->GetWindowWidth();
}

int Tab::GetWindowHeight() const
{
	return _pMaster->GetWindowHeight();
}