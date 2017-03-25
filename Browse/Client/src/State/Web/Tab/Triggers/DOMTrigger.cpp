//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "DOMTrigger.h"
#include "src/State/Web/Tab/Pipelines/TextInputPipeline.h"
#include "src/Singletons/LabStreamMailer.h"
#include <map>

DOMTrigger::DOMTrigger(TabInteractionInterface* pTab, std::shared_ptr<DOMNode> spNode) : Trigger(pTab)
{
    // Save member
    _spDomNode = spNode;

    // Create id, which is unique in overlay
    _overlayButtonId = "dom_trigger_" + std::to_string(_spDomNode->GetId());
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
        //_spNode->GetVisibility() && // DOM node visible --> DEPRECATED 
		!_spDomNode->GetRects().empty() // DOM node has rects
        && _spDomNode->GetRects().front().Width() != 0 && _spDomNode->GetRects().front().Height() != 0; // At least the first rect is bigger than zero
    if(_visible != visible)
    {
        _visible = visible;
        _pTab->SetVisibilityOfFloatingFrameInOverlay(_overlayFrameIndex, _visible);
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

		// Decide which pipline to execute
		switch (_spNode->GetNodeType())
		{
		case DOMNodeType::TextInput:
		{
			LabStreamMailer::instance().Send("Text input started");
			_pTab->PushBackPipeline(
				std::move(
					std::unique_ptr<TextInputPipeline>(
						new TextInputPipeline(
							_pTab,
							_spNode))));
		}
			break;
		case DOMNodeType::SelectField:
			// TODO
			break;
		default:
			LogError("Unkown DOM Node Type as trigger");
		}

		// Return successful activation
        return true;
    }
    else
    {
		// Still waiting for activation
        return false;
    }
}

void DOMTrigger::Draw() const
{
    // Nothing to draw here, since eyeGUI does it
}

void DOMTrigger::Activate()
{
    _pTab->SetVisibilityOfFloatingFrameInOverlay(_overlayFrameIndex, _visible);
}

void DOMTrigger::Deactivate()
{
    _pTab->SetVisibilityOfFloatingFrameInOverlay(_overlayFrameIndex, false);
}

void DOMTrigger::CalculatePositionOfOverlayButton(float& rRelativePositionX, float& rRelativePositionY) const
{
	// Scrolling offset only when not fixed
	double scrollingOffsetX = 0;
	double scrollingOffsetY = 0;
	if (!_spDomNode->GetFixedId())
	{
		_pTab->GetScrollingOffset(scrollingOffsetX, scrollingOffsetY);
	}

	if (_spDomNode->GetRects().size() > 0)
	{
		const auto& rectCenter = _spDomNode->GetRects()[0].Center();
		// Center of node in WebViewPixel space
		double webViewPixelX = rectCenter.x - scrollingOffsetX;
		double webViewPixelY = rectCenter.y - scrollingOffsetY;
		_pTab->ConvertToWebViewPixel(webViewPixelX, webViewPixelY);

		// Calculate coordinates and size
		rRelativePositionX = ((float)webViewPixelX + (float)_pTab->GetWebViewX()) / (float)_pTab->GetWindowWidth();
		rRelativePositionY = ((float)webViewPixelY + (float)_pTab->GetWebViewY()) / (float)_pTab->GetWindowHeight();

		// Subtract half of size to center frame
		rRelativePositionX -= _size / 2.f;
		rRelativePositionY -= _size / 2.f;
	}
}

DOMTextInputTrigger::DOMTextInputTrigger(TabInteractionInterface * pTab, std::shared_ptr<DOMTextInput> spNode)
	: DOMTrigger(pTab, spNode)
{

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
		[]() {}); // no down
}

DOMSelectFieldTrigger::DOMSelectFieldTrigger(TabInteractionInterface * pTab, std::shared_ptr<DOMSelectField> spNode)
	: DOMTrigger(pTab, spNode)
{
	
	// Id mapper for brick
	std::map<std::string, std::string> idMapper;
	idMapper.emplace("button", _overlayButtonId);

	// Calculate position of overlay button
	float x, y;
	CalculatePositionOfOverlayButton(x, y);

	// Add overlay
	_overlayFrameIndex = _pTab->AddFloatingFrameToOverlay("bricks/triggers/TextInput.beyegui", x, y, _size, _size, idMapper);
	_overlayFrameIndex = _pTab->AddFloatingFrameToOverlay("bricks/triggers/SelectField.beyegui", x, y, _size, _size, idMapper);

	// Register listener
	_pTab->RegisterButtonListenerInOverlay(
		_overlayButtonId,
		[&]() { this->_triggered = true; }, // it is checked for triggered in update
		[]() {}); // no down
}
