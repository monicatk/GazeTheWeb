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
	return _sendRenderMessage(msg);
}

CefRefPtr<CefProcessMessage> DOMJavascriptCommunication::SetupExecuteFunctionMessage(
	std::string func_name, 
	CefRefPtr<CefListValue> param)
{
	CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("ExecuteJavascriptFunction");
	const auto& args = msg->GetArgumentList();
	CefRefPtr<CefListValue> header = CefListValue::Create();
	header->SetString(0, func_name);
	header->SetInt(1, GetType());
	header->SetInt(2, GetId());

	args->SetList(0, header);
	args->SetList(1, param);
	return msg;
}


void DOMTextInputInteraction::InputText(std::string text, bool submit)
{
	CefRefPtr<CefListValue> param = CefListValue::Create();
	param->SetString(0, text);
	param->SetBool(1, submit);

	CefRefPtr<CefProcessMessage> msg = SetupExecuteFunctionMessage("setTextInput", param);
	SendProcessMessageToRenderer(msg);
}

void DOMOverflowElementInteraction::Scroll(int x, int y, std::vector<int> fixedIds)
{
	CefRefPtr<CefListValue> param = CefListValue::Create();
	param->SetInt(0, x);
	param->SetInt(1, y);
	CefRefPtr<CefListValue> ids = CefListValue::Create();
	for (const auto& id : fixedIds)
		ids->SetInt(ids->GetSize(), id);
	param->SetList(2, ids);

	CefRefPtr<CefProcessMessage> msg = SetupExecuteFunctionMessage("scroll", param);

	SendProcessMessageToRenderer(msg);
}

void DOMSelectFieldInteraction::SetSelectionIndex(int idx)
{
	CefRefPtr<CefListValue> param = CefListValue::Create();
	param->SetInt(0, idx);
	CefRefPtr<CefProcessMessage> msg = SetupExecuteFunctionMessage("setSelectionIdx", param);
	SendProcessMessageToRenderer(msg);
}

