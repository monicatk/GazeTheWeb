//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Mueller (muellerd@uni-koblenz.de)
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "src/CEF/JSDialogHandler.h"
#include "src/Utils/Logger.h"

bool JSDialogHandler::OnJSDialog(CefRefPtr<CefBrowser> browser,
	const CefString& origin_url,
	const CefString& accept_lang,
	JSDialogType dialog_type,
	const CefString& message_text,
	const CefString& default_prompt_text,
	CefRefPtr<CefJSDialogCallback> callback,
	bool& suppress_message) {

	// NOTE:
	// "If a custom dialog is used the application must execute
	// |callback| once the custom dialog is dismissed."

	// default_prompt_text only used in prompts, obviously.

	// Required
	// alert: single ok button
	// confirm: ok & cancel button
	// prompt: text input field, ok & cancel button


	LogDebug("JSDialogHandler: Dialog information provided ",
		"\ndialog_type: ", dialog_type,
		"\ndefault_prompt_text: ", default_prompt_text.ToString(),
		"\nmsg_txt: ", message_text.ToString(),
		"\naccept_lang: ", accept_lang.ToString(),
		"\norigin_url: ", origin_url.ToString());

	// TODO: Show own dialog popup, see given dialog type & parameters

	/* 
	// HOW TO ANSWER DIALOG CALLBACK
	bool clicked_ok;
	std::string users_answer;
	callback->Continue(clicked_ok, users_answer);

	//return true;
	*/

	return false;
}