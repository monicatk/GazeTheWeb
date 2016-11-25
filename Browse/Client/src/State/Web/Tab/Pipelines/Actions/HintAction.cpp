//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "HintAction.h"
#include "src/State/Web/Tab/Interface/TabInteractionInterface.h"

HintAction::HintAction(TabInteractionInterface *pTab, std::string key, std::string id) : Action(pTab)
{
    // Create id, which is unique in overlay
    _overlayButtonId = "hint_button_" + id;
	_overlayTextBlockId = "hint_text_block_" + id;

    // Id mapper for brick to change ids from file to the used ones
    std::map<std::string, std::string> idMapper;
    idMapper.emplace("button", _overlayButtonId);
	idMapper.emplace("text_block", _overlayTextBlockId);

    // Calculate size of overlay
    float x, y, sizeX, sizeY;
    x = (float)_pTab->GetWebViewX() / (float)_pTab->GetWindowWidth();
    y = (float)_pTab->GetWebViewY() / (float)_pTab->GetWindowHeight();
    sizeX = (float)_pTab->GetWebViewWidth() / (float)_pTab->GetWindowWidth();
    sizeY = (float)_pTab->GetWebViewHeight() / (float)_pTab->GetWindowHeight();

    // Add overlay
    _overlayFrameIndex = _pTab->AddFloatingFrameToOverlay("bricks/actions/Hint.beyegui", x, y, sizeX, sizeY, idMapper);

    // Register listener
    _pTab->RegisterButtonListenerInOverlay(
        _overlayButtonId,
        [&]() // down callback
        {
			_pTab->SetVisibilyOfFloatingFrameInOverlay(_overlayFrameIndex, false);
            this->_done = true;
        },
        [](){}); // up callback

	// Set content of text block
	_pTab->SetContentOfTextBlock(_overlayTextBlockId, key);
}

HintAction::~HintAction()
{
    // Delete overlay frame
    _pTab->RemoveFloatingFrameFromOverlay(_overlayFrameIndex);

    // Unregister button
    _pTab->UnregisterButtonListenerInOverlay(_overlayButtonId);
}

bool HintAction::Update(float tpf, TabInput tabInput)
{
	if(_done)
	{
		if (_waitDuration <= 0)
		{
			return true;
		}
		_waitDuration -= tpf;
	}
    return false;
}

void HintAction::Draw() const
{
	// Nothing to do
}

void HintAction::Activate()
{
	if (!_done)
	{
		_pTab->SetVisibilyOfFloatingFrameInOverlay(_overlayFrameIndex, true);
	}
}

void HintAction::Deactivate()
{
	if (!_done)
	{
		_pTab->SetVisibilyOfFloatingFrameInOverlay(_overlayFrameIndex, false);
	}
}

void HintAction::Abort()
{

}
