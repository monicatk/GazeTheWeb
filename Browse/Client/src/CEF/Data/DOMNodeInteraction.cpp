//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Mueller (muellerd@uni-koblenz.de)
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "DOMNodeInteraction.h"
#include "src/Utils/Logger.h"
#include "src/State/Web/Tab/Tab.h"

bool DOMJavascriptCommunication::SendProcessMessageToRenderer(CefRefPtr<CefProcessMessage> msg)
{
	if (_pTab)
	{
		return _pTab->SendProcessMessageToRenderer(msg);
	}
	return false;
}


void DOMTextInputInteraction::InputText(std::string text, bool submit)
{
	CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("ExecuteJavascriptFunction");
	const auto& args = msg->GetArgumentList();
	//CefRefPtr<CefListValue> 
	args->SetString(0, "PerformTextInput");
	args->SetInt(1, GetId());
	args->SetString(2, text);
	args->SetBool(3, submit);

	SendProcessMessageToRenderer(msg);
}

void DOMOverflowElementInteraction::Scroll(int x, int y, std::vector<int> fixedIds)
{
	CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("ExecuteJavascriptFunction");
	const auto& args = msg->GetArgumentList();

	args->SetString(0, "ScrollOverflowElement");
	args->SetInt(1, GetId());
	args->SetInt(2, x);
	args->SetInt(3, y);

	CefRefPtr<CefListValue> ids = CefListValue::Create();
	for (const auto& id : fixedIds)
		ids->SetInt(ids->GetSize(), id);

	args->SetList(4, ids);

	SendProcessMessageToRenderer(msg);
}

void DOMSelectFieldInteraction::SetSelectionIndex(int idx)
{
	CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("SetSelectionIndex");
	CefRefPtr<CefListValue> args = msg->GetArgumentList();
	args->SetInt(0, GetId());
	args->SetInt(1, idx);
	SendProcessMessageToRenderer(msg);
}

