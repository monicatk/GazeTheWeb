//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "LeftMouseButtonClickPipeline.h"
#include "src/State/Web/Tab/Pipelines/Actions/LeftMouseButtonClickAction.h"

LeftMouseButtonClickPipeline::LeftMouseButtonClickPipeline(TabInteractionInterface* pTab, glm::vec2 coordinate) : Pipeline(pTab)
{
	// Add action and set coordinate
    std::unique_ptr<LeftMouseButtonClickAction> upLeftMouseButtonClickAction =
        std::unique_ptr<LeftMouseButtonClickAction>(new LeftMouseButtonClickAction(_pTab));
	upLeftMouseButtonClickAction->SetInputValue("coordinate", coordinate);
    _actions.push_back(std::move(upLeftMouseButtonClickAction));
}
