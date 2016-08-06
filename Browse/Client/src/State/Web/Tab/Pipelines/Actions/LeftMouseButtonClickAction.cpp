//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "LeftMouseButtonClickAction.h"
#include "src/State/Web/Tab/Interface/TabInteractionInterface.h"

LeftMouseButtonClickAction::LeftMouseButtonClickAction(TabInteractionInterface* pTab) : Action(pTab)
{
    // Add in- and output data slots
    AddVec2InputSlot("coordinate");
}

bool LeftMouseButtonClickAction::Update(float tpf, TabInput tabInput)
{
    // Extract size of webview to calculate pixel coordinates
    int width, height;
    _pTab->GetWebViewTextureResolution(width, height);

    // Get coordinate from input slot
    glm::vec2 coordinate;
    GetInputValue("coordinate", coordinate);

    // Emulate left mouse button click
    if (width > 0 && height > 0)
    {
        _pTab->EmulateLeftMouseButtonClick((double)(coordinate.x * width), (double)(coordinate.y * height));
    }

    return true;
}

void LeftMouseButtonClickAction::Draw() const
{
    // Nothing to do
}

void LeftMouseButtonClickAction::Activate()
{
    // Nothing to do
}

void LeftMouseButtonClickAction::Deactivate()
{
    // Nothing to do
}

void LeftMouseButtonClickAction::Abort()
{
    // Nothing to do
}
