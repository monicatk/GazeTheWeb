//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "JSDialogPipeline.h"
#include "src/State/Web/Tab/Pipelines/Actions/HintAction.h"
#include "src/State/Web/Tab/Interface/TabInteractionInterface.h"

JSDialogPipeline::JSDialogPipeline(TabInteractionInterface* pTab, JavaScriptDialogType type, std::string message) : Pipeline(pTab)
{
	// TODO: Handle different types
	auto spHintAction = std::make_shared<HintAction>(_pTab, "hint", "JSDialog");
	_actions.push_back(spHintAction);
	pTab->ReplyJSDialog(false, "");
}
