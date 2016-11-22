//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "TextSelectionAction.h"
#include "src/State/Web/Tab/Interface/TabInteractionInterface.h"

TextSelectionAction::TextSelectionAction(TabInteractionInterface *pTab) : Action(pTab)
{
}

bool TextSelectionAction::Update(float tpf, TabInput tabInput)
{
	bool done = false;
    return done;
}

void TextSelectionAction::Draw() const
{

}

void TextSelectionAction::Activate()
{

}

void TextSelectionAction::Deactivate()
{

}

void TextSelectionAction::Abort()
{

}
