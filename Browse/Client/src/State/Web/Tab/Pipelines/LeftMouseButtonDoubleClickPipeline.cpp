//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "LeftMouseButtonDoubleClickPipeline.h"
#include "src/State/Web/Tab/Pipelines/Actions/LeftMouseButtonClickAction.h"
#include "src/State/Web/Tab/Pipelines/Actions/DelayAction.h"

LeftMouseButtonDoubleClickPipeline::LeftMouseButtonDoubleClickPipeline(TabInteractionInterface* pTab, glm::vec2 coordinate) : Pipeline(pTab)
{
	// Add first left click action and set coordinate
    auto upLeftMouseButtonClickAction1 = std::make_unique<LeftMouseButtonClickAction>(_pTab);
	upLeftMouseButtonClickAction1->SetInputValue("coordinate", coordinate);
    _actions.push_back(std::move(upLeftMouseButtonClickAction1));

	// Add delay between clicks
	auto upDelayAction = std::make_unique<DelayAction>(_pTab, 0.1f);
	_actions.push_back(std::move(upDelayAction));

	// Add second left click action and set coordinate
	auto upLeftMouseButtonClickAction2 = std::make_unique<LeftMouseButtonClickAction>(_pTab);
	upLeftMouseButtonClickAction2->SetInputValue("coordinate", coordinate);
	_actions.push_back(std::move(upLeftMouseButtonClickAction2));
}
