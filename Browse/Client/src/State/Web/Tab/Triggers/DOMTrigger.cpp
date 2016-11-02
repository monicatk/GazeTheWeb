//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "DOMTrigger.h"
#include "src/State/Web/Tab/Pipelines/TextInputPipeline.h"
#include <map>

DOMTrigger::DOMTrigger(TabInteractionInterface* pTab, std::shared_ptr<DOMNode> spNode) : Trigger(pTab)
{
    // Save member
    _spNode = spNode;

    // TODO: Check type of DOMNode

    // Create id, which is unique in overlay
    _overlayButtonId = "dom_trigger" + std::to_string(_spNode->GetFrameID()) + "_" + std::to_string(_spNode->GetNodeID());

    // Id mapper for brick
    std::map<std::string, std::string> idMapper;
    idMapper.emplace("button", _overlayButtonId);

    // Calculate position of overlay button
    float x, y;
    CalculatePositionOfOverlayButton(x, y);

    // Add overlay
    _overlayFrameIndex = _pTab->AddFloatingFrameToOverlay("bricks/triggers/TextInput.beyegui", x, y, _size, _size, idMapper);

    // Register listener
    _pTab->RegisterButtonListenerInOverlay(
		_overlayButtonId,
		[&]() { this->_triggered = true; }, // it is checked for triggered in update
		[](){}); // no down
}

DOMTrigger::~DOMTrigger()
{
    // Delete overlay frame
    _pTab->RemoveFloatingFrameFromOverlay(_overlayFrameIndex);

	// Unregister button from overlay
	_pTab->UnregisterButtonListenerInOverlay(_overlayButtonId);
}

bool DOMTrigger::Update(float tpf, TabInput& rTabInput)
{
    // Decide visibility
    bool visible =
        _spNode->GetVisibility() // DOM node visible
        && !_spNode->GetRects().empty() // DOM node has rects
        && _spNode->GetRects().front().width() != 0 && _spNode->GetRects().front().height() != 0; // At least the first rect is bigger than zero
    if(_visible != visible)
    {
        _visible = visible;
        _pTab->SetVisibilyOfFloatingFrameInOverlay(_overlayFrameIndex, _visible);
    }

    // Calculate position of overlay button
    float x, y;
    CalculatePositionOfOverlayButton(x, y);

    // Tell it floating frame
    _pTab->SetPositionOfFloatingFrameInOverlay(_overlayFrameIndex, x, y);

    // Return whether triggered
    if(_triggered)
    {
        _triggered = false;
		_pTab->PushBackPipeline(
			std::move(
				std::unique_ptr<TextInputPipeline>(
					new TextInputPipeline(
						_pTab,
						_spNode))));
        return true;
    }
    else
    {
        return false;
    }
}

void DOMTrigger::Draw() const
{
    // Nothing to draw here, since eyeGUI does it
}

void DOMTrigger::Activate()
{
    _pTab->SetVisibilyOfFloatingFrameInOverlay(_overlayFrameIndex, _visible);
}

void DOMTrigger::Deactivate()
{
    _pTab->SetVisibilyOfFloatingFrameInOverlay(_overlayFrameIndex, false);
}

void DOMTrigger::CalculatePositionOfOverlayButton(float& rRelativePositionX, float& rRelativePositionY) const
{
    // Center of node
	glm::vec2 center = _spNode->GetCenter();

	// Scale center from web view resolution to real pixel value
	center.x = (center.x / (float)_pTab->GetWebViewResolutionX()) * (float)_pTab->GetWebViewWidth();
	center.y = (center.y / (float)_pTab->GetWebViewResolutionY()) * (float)_pTab->GetWebViewHeight();

    // Scrolling offset only when not fixed
	double scrollingOffsetX = 0;
	double scrollingOffsetY = 0;
	if (!_spNode->GetFixed())
	{
		_pTab->GetScrollingOffset(scrollingOffsetX, scrollingOffsetY);
	}

    // Calculate coordinates and size
    rRelativePositionX = (center.x - scrollingOffsetX + (float)_pTab->GetWebViewX()) / (float)_pTab->GetWindowWidth();
    rRelativePositionY = (center.y - scrollingOffsetY + (float)_pTab->GetWebViewY()) / (float)_pTab->GetWindowHeight();

    // Subtract half of size to center frame
    rRelativePositionX -= _size / 2.f;
    rRelativePositionY -= _size / 2.f;
}
