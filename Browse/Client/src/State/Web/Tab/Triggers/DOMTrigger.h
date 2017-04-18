//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Abstract trigger generated from DOM elements. Template structure to be used
// as trigger from various DOM node types.

#ifndef DOMTRIGGER_H_
#define DOMTRIGGER_H_

#include "src/State/Web/Tab/Triggers/Trigger.h"
#include "src/CEF/Data/DOMNode.h"
#include "submodules/eyeGUI/include/eyeGUI.h"
#include <memory>

template <class T>
class DOMTrigger : public Trigger
{
public:

    // Constructor
	DOMTrigger(TabInteractionInterface* pTab, std::shared_ptr<T> spNode, std::string brickPath);

    // Destructor
    virtual ~DOMTrigger() = 0;

    // Update
    virtual bool Update(float tpf, TabInput& rTabInput);

    // Draw
    virtual void Draw() const;

    // Activate
    virtual void Activate();

    // Deactivate
    virtual void Deactivate();

	// Get rects of DOMNode
    std::vector<Rect> GetDOMRects() const { return _spNode->GetRects(); }

    // Get whether DOMNode is marked as fixed
    bool GetDOMFixed() const { return _spNode->GetFixedId(); } // TODO: call real "isFixed" method so not checked for being zero

protected:

	// Shared pointer to node
	std::shared_ptr<T> _spNode;

private:

    // Calculate position of overlay button
    void CalculatePositionOfOverlayButton(float& rRelativePositionX, float& rRelativePositionY) const;

    // Index of floating frame in Tab's overlay
    int _overlayFrameIndex = -1;

    // Id of button in overlay
    std::string _overlayButtonId;

    // Size of overlay button
    float _size = 0.15f;

    // Bool to remember that it was triggered
    bool _triggered = false;

    // Visibility of overlay
    bool _visible = false;
};

// ######################
// ### IMPLEMENTATION ###
// ######################

template <class T>
DOMTrigger<T>::DOMTrigger(TabInteractionInterface* pTab, std::shared_ptr<T> spNode, std::string brickPath) : Trigger(pTab)
{
	// Save member
	_spNode = spNode;

	// Create id, which is unique in overlay
	_overlayButtonId = "dom_trigger_" + std::to_string(_spNode->GetId());

	// Id mapper for brick
	std::map<std::string, std::string> idMapper;
	idMapper.emplace("button", _overlayButtonId);

	// Calculate position of overlay button
	float x, y;
	CalculatePositionOfOverlayButton(x, y);

	// Add overlay
	_overlayFrameIndex = _pTab->AddFloatingFrameToOverlay(brickPath, x, y, _size, _size, idMapper);

	// Register listener
	_pTab->RegisterButtonListenerInOverlay(
		_overlayButtonId,
		[&]() { this->_triggered = true; }, // it is checked for triggered in update
		[]() {}); // no down
}

template <class T>
DOMTrigger<T>::~DOMTrigger()
{
	// Delete overlay frame
	_pTab->RemoveFloatingFrameFromOverlay(_overlayFrameIndex);

	// Unregister button from overlay
	_pTab->UnregisterButtonListenerInOverlay(_overlayButtonId);
}

template <class T>
bool DOMTrigger<T>::Update(float tpf, TabInput& rTabInput)
{
	// Decide visibility (TODO: no visibility variable in DOMNode anymore?)
	bool visible =
		!_spNode->GetRects().empty() // DOM node has rects
		&& _spNode->GetRects().front().Width() != 0 && _spNode->GetRects().front().Height() != 0; // At least the first rect is bigger than zero
	if (_visible != visible)
	{
		_visible = visible;
		_pTab->SetVisibilityOfFloatingFrameInOverlay(_overlayFrameIndex, _visible);
	}

	// Calculate position of overlay button
	float x, y;
	CalculatePositionOfOverlayButton(x, y);

	// Tell it floating frame
	_pTab->SetPositionOfFloatingFrameInOverlay(_overlayFrameIndex, x, y);

	// Return true whether triggered
	return _triggered;
}

template <class T>
void DOMTrigger<T>::Draw() const
{
	// Nothing to draw here, since eyeGUI does it
}

template <class T>
void DOMTrigger<T>::Activate()
{
	_pTab->SetVisibilityOfFloatingFrameInOverlay(_overlayFrameIndex, _visible);
}

template <class T>
void DOMTrigger<T>::Deactivate()
{
	_pTab->SetVisibilityOfFloatingFrameInOverlay(_overlayFrameIndex, false);
}

template <class T>
void DOMTrigger<T>::CalculatePositionOfOverlayButton(float& rRelativePositionX, float& rRelativePositionY) const
{
	// Scrolling offset only when not fixed
	double scrollingOffsetX = 0;
	double scrollingOffsetY = 0;
	if (!_spNode->GetFixedId())
	{
		_pTab->GetScrollingOffset(scrollingOffsetX, scrollingOffsetY);
	}

	if (_spNode->GetRects().size() > 0)
	{
		const auto& nodeCenter = _spNode->GetRects()[0].Center();

		// Center of node in WebViewPixel space
		double webViewPixelX = nodeCenter.x - scrollingOffsetX;
		double webViewPixelY = nodeCenter.y - scrollingOffsetY;
		_pTab->ConvertToWebViewPixel(webViewPixelX, webViewPixelY);

		// Calculate coordinates and size
		rRelativePositionX = ((float)webViewPixelX + (float)_pTab->GetWebViewX()) / (float)_pTab->GetWindowWidth();
		rRelativePositionY = ((float)webViewPixelY + (float)_pTab->GetWebViewY()) / (float)_pTab->GetWindowHeight();

		// Subtract half of size to center frame
		rRelativePositionX -= _size / 2.f;
		rRelativePositionY -= _size / 2.f;
	}
}

#endif // DOMTRIGGER_H_
