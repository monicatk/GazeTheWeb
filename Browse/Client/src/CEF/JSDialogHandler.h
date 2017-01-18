//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Mueller (muellerd@uni-koblenz.de)
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#ifndef JSDIALOGHANDLER_H_
#define JSDIALOGHANDLER_H_

#include "include/cef_jsdialog_handler.h"

class JSDialogHandler : public CefJSDialogHandler
{
public:

	JSDialogHandler() {};
	~JSDialogHandler() {};

	bool OnJSDialog(CefRefPtr<CefBrowser> browser,
		const CefString& origin_url,
		const CefString& accept_lang,
		JSDialogType dialog_type,
		const CefString& message_text,
		const CefString& default_prompt_text,
		CefRefPtr<CefJSDialogCallback> callback,
		bool& suppress_message) OVERRIDE;

private:



    // Include CEF'S default reference counting implementation
    IMPLEMENT_REFCOUNTING(JSDialogHandler);
};



#endif  // JSDIALOGHANDLER_H_
