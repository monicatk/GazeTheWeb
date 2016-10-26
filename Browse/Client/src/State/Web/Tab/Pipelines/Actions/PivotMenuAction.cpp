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
    int windowWidth, windowHeight;
    _pTab->GetWindowSize(windowWidth, windowHeight);
    float x, y, sizeX, sizeY;

    // ### Menu overlay ###

    // Pixel values
    x = (float)webViewX + ((float)webViewWidth * 0.1f);
    y = (float)webViewY + ((float)webViewHeight * 0.7f);
    sizeX = (float)webViewWidth * 0.8f;
    sizeY = (float)webViewHeight * 0.2f;

    // Relative values
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

    // ### Pivot overlay ###

    // Set position and size at update since here coordinate is not known

    _pivotFrameIndex = _pTab->AddFloatingFrameToOverlay(
        "bricks/actions/Pivot.beyegui",
        0,
        0,
        _pivotSize, // size in x direction
        _pivotSize, // size in y direction
        std::map<std::string, std::string>());
}

PivotMenuAction::~PivotMenuAction()
{
    // Delete overlay frames
    _pTab->RemoveFloatingFrameFromOverlay(_menuFrameIndex);
    _pTab->RemoveFloatingFrameFromOverlay(_pivotFrameIndex);
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
    // Use coordinate to set position of cursor
    glm::vec2 coordinate;
    GetInputValue("coordinate", coordinate);

    int webViewX, webViewY, webViewWidth, webViewHeight;
    _pTab->CalculateWebViewPositionAndSize(webViewX, webViewY, webViewWidth, webViewHeight);
    int windowWidth, windowHeight;
    _pTab->GetWindowSize(windowWidth, windowHeight);

    coordinate.x += webViewX;
    coordinate.y += webViewY;
    coordinate.x /= windowWidth;
    coordinate.y /= windowHeight;
    coordinate.x -= _pivotSize / 2.f;
    coordinate.y -= _pivotSize / 2.f;

    _pTab->SetPositionOfFloatingFrameInOverlay(_pivotFrameIndex, coordinate.x, coordinate.y);

    // Make overlays visible
    _pTab->SetVisibilyOfFloatingFrameInOverlay(_menuFrameIndex, true);
    _pTab->SetVisibilyOfFloatingFrameInOverlay(_pivotFrameIndex, true);
}

void PivotMenuAction::Deactivate()
{
    _pTab->SetVisibilyOfFloatingFrameInOverlay(_menuFrameIndex, false);
    _pTab->SetVisibilyOfFloatingFrameInOverlay(_pivotFrameIndex, false);
}

void PivotMenuAction::Abort()
{
    // Nothing to do
}
