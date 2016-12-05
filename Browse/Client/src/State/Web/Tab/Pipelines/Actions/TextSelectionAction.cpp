//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "TextSelectionAction.h"
#include "src/State/Web/Tab/Interface/TabInteractionInterface.h"
#include "src/Setup.h"

TextSelectionAction::TextSelectionAction(TabInteractionInterface *pTab) : ZoomCoordinateAction(pTab, false)
{
	// Add in- and output data slots
	AddVec2InputSlot("coordinate");
}

bool TextSelectionAction::Update(float tpf, TabInput tabInput)
{
	// Call standard zoom coordinate update function
	bool done = ZoomCoordinateAction::Update(tpf, tabInput);

	// Calculate current coordinate in WebViewPixel space
	glm::vec2 webViewPixels(_pTab->GetWebViewWidth(), _pTab->GetWebViewHeight());
	glm::vec2 coordinate = _coordinate * webViewPixels; // _coordinate is given by superclass

	// When finished, set end position of text selection
	if (done)
	{
		// End selection procedure
		_pTab->EmulateLeftMouseButtonUp(coordinate.x, coordinate.y, true, setup::TEXT_SELECTION_MARGIN);

		// Copy selected string to clipboard. Maybe create extra action for this later
		_pTab->PutTextSelectionToClipboardAsync();
	}
	else
	{
		// Keep emulating mouse cursor
		_pTab->EmulateMouseCursor(coordinate.x, coordinate.y, true, setup::TEXT_SELECTION_MARGIN);
	}

    return done;
}

void TextSelectionAction::Activate()
{
	// Call super method
	ZoomCoordinateAction::Activate();

	// Set starting point of selection
	glm::vec2 startCoordinate;
	GetInputValue("coordinate", startCoordinate);
	_pTab->EmulateLeftMouseButtonDown(startCoordinate.x, startCoordinate.y, true, -setup::TEXT_SELECTION_MARGIN);
}

void TextSelectionAction::Deactivate()
{
    // Reset mouse state
    _pTab->EmulateLeftMouseButtonUp(0, 0);
}
