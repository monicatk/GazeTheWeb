//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "TextSelectionAction.h"
#include "src/State/Web/Tab/Interface/TabInteractionInterface.h"

TextSelectionAction::TextSelectionAction(TabInteractionInterface *pTab) : ZoomCoordinateAction(pTab)
{
	// Add in- and output data slots
	AddVec2InputSlot("coordinate");
}

bool TextSelectionAction::Update(float tpf, TabInput tabInput)
{
	// Call standard zoom coordinate update function
	bool done = ZoomCoordinateAction::Update(tpf, tabInput);

	// TODO: maybe use some more direct coordinate than this one
	// Calculate current coordinate in screen space
	glm::vec2 webViewPixels(_pTab->GetWebViewWidth(), _pTab->GetWebViewHeight());
	glm::vec2 screenCoordinate = _coordinate * webViewPixels;

	// When finished, set end position of text selection
	if (done)
	{
		// End selection procedure
		_pTab->EmulateLeftMouseButtonUp(screenCoordinate.x, screenCoordinate.y);

		// Copy selected string to clipboard. Maybe create extra action for this later
		_pTab->PutTextSelectionToClipboardAsync();
	}
	else
	{
		// Keep emulating mouse cursor
		_pTab->EmulateMouseCursor(screenCoordinate.x, screenCoordinate.y, true);
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
	_pTab->EmulateLeftMouseButtonDown(startCoordinate.x, startCoordinate.y);
}