//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "PivotMenuAction.h"
#include "src/State/Web/Tab/Interface/TabInteractionInterface.h"

PivotMenuAction::PivotMenuAction(TabInteractionInterface *pTab) : Action(pTab)
{
    // Add in- and output data slots
    AddVec2InputSlot("coordinate");

    // Calculate size of overlay (TODO: what to do at resize? everything ok? simpler function call would be cool)
    int webViewX, webViewY, webViewWidth, webViewHeight;
    _pTab->CalculateWebViewPositionAndSize(webViewX, webViewY, webViewWidth, webViewHeight);
    float x, y, sizeX, sizeY;

    // Pixel values
    x = (float)webViewX + ((float)webViewWidth * 0.1f);
    y = (float)webViewY + ((float)webViewHeight * 0.7f);
    sizeX = (float)webViewWidth * 0.8f;
    sizeY = (float)webViewHeight * 0.2f;

    // Relative values
    int windowWidth, windowHeight;
    _pTab->GetWindowSize(windowWidth, windowHeight);
    x /= windowWidth;
    y /= windowHeight;
    sizeX /= windowWidth;
    sizeY /= windowHeight;

    // Add overlay (TODO: no id mapper since no idea when there could occur a problem about it)
    _menuFrameIndex = _pTab->AddFloatingFrameToOverlay(
        "bricks/actions/PivotMenu.beyegui",
        x,
        y,
        sizeX,
        sizeY,
        std::map<std::string, std::string>());
}

PivotMenuAction::~PivotMenuAction()
{
    // Delete overlay frames
    _pTab->RemoveFloatingFrameFromOverlay(_menuFrameIndex);
}

bool PivotMenuAction::Update(float tpf, TabInput tabInput)
{
    bool done = false;
    return done;
}

void PivotMenuAction::Draw() const
{
    // Nothing to do
}

void PivotMenuAction::Activate()
{
    _pTab->SetVisibilyOfFloatingFrameInOverlay(_menuFrameIndex, true);
}

void PivotMenuAction::Deactivate()
{
    _pTab->SetVisibilyOfFloatingFrameInOverlay(_menuFrameIndex, false);
}

void PivotMenuAction::Abort()
{
    // Nothing to do
}
