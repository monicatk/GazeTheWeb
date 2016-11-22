//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "TextSelectionAction.h"
#include "src/State/Web/Tab/Interface/TabInteractionInterface.h"

TextSelectionAction::TextSelectionAction(TabInteractionInterface *pTab) : Action(pTab)
{
	// TODO: just some timer to end action
	_timer = 5.f;
}

bool TextSelectionAction::Update(float tpf, TabInput tabInput)
{
	bool done = false;

	_timer -= tpf;

	// When finished, set end position of text selection
	if (_timer < 0)
	{
		// End selection procedure
		_pTab->EndTextSelection(tabInput.webViewGazeX, tabInput.webViewGazeY);
		done = true;
	}
	else
	{
		// Keep simulating mouse cursor
		_pTab->EmulateMouseCursor(tabInput.webViewGazeX, tabInput.webViewGazeY);
	}

    return done;
}

void TextSelectionAction::Draw() const
{

}

void TextSelectionAction::Activate()
{
	// Set starting point of selection for testing purposes
	_pTab->StartTextSelection(310, 470);
}

void TextSelectionAction::Deactivate()
{

}

void TextSelectionAction::Abort()
{

}
