//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "TextSelectionPipeline.h"
#include "src/State/Web/Tab/Pipelines/Actions/TextSelectionAction.h"

TextSelectionPipeline::TextSelectionPipeline(TabInteractionInterface* pTab) : Pipeline(pTab)
{
	// Add text selection action
	auto upTextSelectionAction = std::make_unique<TextSelectionAction>(_pTab);
	upTextSelectionAction->SetInputValue("coordinate", glm::vec2(310, 470)); // set start coordinate for selection
	_actions.push_back(std::move(upTextSelectionAction));
}
