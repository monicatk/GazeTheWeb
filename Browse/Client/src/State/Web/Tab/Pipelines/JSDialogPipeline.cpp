//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "JSDialogPipeline.h"
#include "src/State/Web/Tab/Pipelines/Actions/JSDialogAction.h"
#include "src/State/Web/Tab/Pipelines/Actions/KeyboardAction.h"
#include "src/State/Web/Tab/Pipelines/Actions/ReplyJSDialogAction.h"
#include "src/State/Web/Tab/Interface/TabInteractionInterface.h"

JSDialogPipeline::JSDialogPipeline(TabInteractionInterface* pTab, JavaScriptDialogType type, std::string message) : Pipeline(pTab)
{
	// Dialog to user
	std::shared_ptr<JSDialogAction> spJSDialogAction;

	// Reply to JavaScript dialog
	auto spReplyJSDialogAction = std::make_shared<ReplyJSDialogAction>(_pTab);

	// Decide what to do depeding on type of JavaScript Dialog type
	if (type == JavaScriptDialogType::ALERT) // Ok
	{
		// JavaScript dialog action with deactivated cancel button
		spJSDialogAction = std::make_shared<JSDialogAction>(_pTab, message, false);
		_actions.push_back(spJSDialogAction);
	}
	else if (type == JavaScriptDialogType::PROMPT) // Ok, cancel and text input
	{
		// JavaScript dialog action with activated cancel button
		spJSDialogAction = std::make_shared<JSDialogAction>(_pTab, message, true);
		_actions.push_back(spJSDialogAction);

		// Keyboard for user input
		auto spKeyboardAction = std::make_shared<KeyboardAction>(_pTab);
		_actions.push_back(spKeyboardAction);

		// Connect keyboard and reply
		std::unique_ptr<ActionConnector> upConnector =
			std::unique_ptr<ActionConnector>(new ActionConnector(spKeyboardAction, spReplyJSDialogAction));
		upConnector->ConnectString16("text", "userInput");
		_connectors.push_back(std::move(upConnector));
	}
	else // Ok or cancel
	{
		// JavaScript dialog action with activated cancel button
		spJSDialogAction = std::make_shared<JSDialogAction>(_pTab, message, true);
		_actions.push_back(spJSDialogAction);
	}

	// Push back reply
	_actions.push_back(spReplyJSDialogAction);

	// Connect dialog and reply
	std::unique_ptr<ActionConnector> upConnector =
		std::unique_ptr<ActionConnector>(new ActionConnector(spJSDialogAction, spReplyJSDialogAction));
	upConnector->ConnectInt("clickedOk", "clickedOk");
	_connectors.push_back(std::move(upConnector));	
}
