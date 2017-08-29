//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Mueller (muellerd@uni-koblenz.de)
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "DOMNodeInteraction.h"
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

