//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "JSDialogPipeline.h"
#include "src/State/Web/Tab/Pipelines/Actions/HintAction.h"
#include "src/State/Web/Tab/Interface/TabInteractionInterface.h"

JSDialogPipeline::JSDialogPipeline(TabInteractionInterface* pTab, JavaScriptDialogType type, std::string message) : Pipeline(pTab)
{
	// Decide what to do depeding on type of JavaScript Dialog type
	if (type == JavaScriptDialogType::ALERT)
	{
		// Ok
	}
	if (type == JavaScriptDialogType::PROMPT)
	{
		// Ok, cancel and text input
	}
	else
	{
		// Ok or cancel
	}

	// TODO
	auto spHintAction = std::make_shared<HintAction>(_pTab, "general:not_implemented", "JSDialog");
	_actions.push_back(spHintAction);
	pTab->ReplyJSDialog(false, "");
}
