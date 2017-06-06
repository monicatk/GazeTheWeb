//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "SelectFieldPipeline.h"
#include "src/State/Web/Tab/Interface/TabInteractionInterface.h"
#include "src/State/Web/Tab/Interface/TabActionInterface.h"
#include "src/State/Web/Tab/Pipelines/Actions/SelectFieldOptionsAction.h"
#include "src/State/Web/Tab/Pipelines/Actions/SelectFieldAction.h"
#include "submodules/eyeGUI/include/eyeGUI.h"

SelectFieldPipeline::SelectFieldPipeline(
	TabInteractionInterface* pTab,
	std::shared_ptr<const DOMSelectField> spNode,
	std::shared_ptr<DOMSelectFieldInteraction> spInteractionNode) : Pipeline(pTab)
{
	// Show options to user
	std::shared_ptr<SelectFieldOptionsAction> spOptionsAction = std::make_shared<SelectFieldOptionsAction>(_pTab, spNode);
	_actions.push_back(spOptionsAction);

	// Select one
	std::shared_ptr<SelectFieldAction> spSelectAction = std::make_shared<SelectFieldAction>(_pTab, spInteractionNode);
	_actions.push_back(spSelectAction);

	// Connect actions to pipe selection
	std::unique_ptr<ActionConnector> upConnector =
		std::unique_ptr<ActionConnector>(new ActionConnector(spOptionsAction, spSelectAction));
	upConnector->ConnectInt("option", "option");
	_connectors.push_back(std::move(upConnector));
}
