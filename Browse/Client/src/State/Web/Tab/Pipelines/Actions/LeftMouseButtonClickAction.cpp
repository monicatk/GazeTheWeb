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
	AddIntInputSlot("visualize");

	// Set defaults (TODO: maybe directly at slot creation?)
	SetInputValue("visualize", 1);
}

bool LeftMouseButtonClickAction::Update(float tpf, TabInput tabInput)
{
    // Get coordinate from input slot
    glm::vec2 coordinate;
    GetInputValue("coordinate", coordinate);

	// Get whether should be visualized
	int visualize;
	GetInputValue("visualize", visualize);

    // Emulate left mouse button click
    _pTab->EmulateLeftMouseButtonClick((double)(coordinate.x), (double)(coordinate.y), visualize > 0);

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
