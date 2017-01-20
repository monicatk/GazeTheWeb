//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Mueller (muellerd@uni-koblenz.de)
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "src/CEF/RequestHandler.h"
#include "src/Utils/Logger.h"

CefRequestHandler::ReturnValue RequestHandler::OnBeforeResourceLoad(
	CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	CefRefPtr<CefRequest> request,
	CefRefPtr<CefRequestCallback> callback)
{
	// LogDebug("browser: ", browser->GetIdentifier(), ", frame: ", frame->GetIdentifier(), ", url=", request->GetURL().ToString());
	
	// TODO: Cancel loading of ads
	// return RV_CANCEL in this case!

	return RV_CONTINUE;
}