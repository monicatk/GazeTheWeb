//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "LinkNavigationAction.h"
#include "src/State/Web/Tab/Interface/TabInteractionInterface.h"
#include "src/State/Web/Tab/DOMNode.h"
#include "src/Setup.h"

LinkNavigationAction::LinkNavigationAction(TabInteractionInterface* pTab) : Action(pTab)
{
    // Add in- and output data slots
    AddVec2InputSlot("coordinate");
	AddIntInputSlot("visualize");

	// Set defaults (TODO: maybe directly at slot creation?)
	SetInputValue("visualize", 1);
}

bool LinkNavigationAction::Update(float tpf, TabInput tabInput)
{
    // Get coordinate from input slot
    glm::vec2 coordinate;
    GetInputValue("coordinate", coordinate);

	// Get whether should be visualized
	int visualize;
	GetInputValue("visualize", visualize);

    // Decide what to click
    float distance = 0.f;
    double scrollingX, scrollingY;
    _pTab->GetScrollingOffset(scrollingX, scrollingY);
    glm::vec2 scrolling = glm::vec2(scrollingX, scrollingY);
    glm::vec2 pageCoordinate = coordinate + scrolling;
    std::weak_ptr<const DOMNode> wpNearestLink = _pTab->GetNearestLink(pageCoordinate, distance);

    // Determine where to click instead, if not too far away or coordinate already valid
    if(distance < setup::LINK_CORRECTION_MAX_PIXEL_DISTANCE && distance > 0)
    {
        // Try to get value from weak pointer
        if (auto sp = wpNearestLink.lock())
        {
            // Just get coordinate of first rect
            if(!sp->GetRects().empty())
            {
                glm::vec2 linkCoordinate = sp->GetRects().front().center();
                coordinate = linkCoordinate - scrolling;
            }
        }
    }

    // Emulate left mouse button click
    _pTab->EmulateLeftMouseButtonClick((double)(coordinate.x), (double)(coordinate.y), visualize > 0);

    return true;
}

void LinkNavigationAction::Draw() const
{
    // Nothing to do
}

void LinkNavigationAction::Activate()
{
    // Nothing to do
}

void LinkNavigationAction::Deactivate()
{
    // Nothing to do
}

void LinkNavigationAction::Abort()
{
    // Nothing to do
}
