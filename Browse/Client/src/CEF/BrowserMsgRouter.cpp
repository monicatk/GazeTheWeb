//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Müller (muellerd@uni-koblenz.de)
//============================================================================

#include "BrowserMsgRouter.h"
#include "src/CEF/Extension/CefMediator.h"
#include "src/Utils/Logger.h"
#include <cstdlib>

BrowserMsgRouter::BrowserMsgRouter(CefMediator* pMediator)
{
	_pMediator = pMediator;

	// Create configuration for browser side message router
	CefMessageRouterConfig config;
	config.js_query_function = "cefQuery";
	config.js_cancel_function = "cefQueryCancel";

	// Create and add the core message router
	_router = CefMessageRouterBrowserSide::Create(config);

	// Create and add msgRouter for msg handling
	CefMessageRouterBrowserSide::Handler* myHandler = new MsgHandler(this);
	_router->AddHandler(myHandler, true);
}

MsgHandler::MsgHandler(BrowserMsgRouter* pMsgRouter)
{
	_pMsgRouter = pMsgRouter;
}

bool MsgHandler::OnQuery(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	int64 query_id,
	const CefString& request,
	bool persistent,
	CefRefPtr<Callback> callback)
{
	const std::string requestName = request.ToString();

	if (requestName == "faviconBytesReady")
	{
		callback->Success("GetFavIconBytes");
		LogDebug("BrowserMsgRouter: Received 'faviconBytesReady 'callback from Javascript");

		// Tell renderer to read out favicon image's bytes
		CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("GetFavIconBytes");
		browser->SendProcessMessage(PID_RENDERER, msg);

		return true;
	}

	// Fixed element callbacks
	if (requestName.compare(0, 9, "#fixElem#") == 0)
	{
		if (requestName.compare(9, 4, "rem#") == 0)
		{
			std::string id = requestName.substr(13, 2);
			LogDebug("BrowserMsgRouter: Fixed element #", id, " was removed.");

			// Notify Tab via CefMediator, that a fixed element was removed
			_pMsgRouter->GetMediator()->RemoveFixedElement(browser, atoi(id.c_str()));

			return true;
		}
		if (requestName.compare(9, 4, "add#") == 0)
		{
			std::string id = requestName.substr(13, 2);
			LogDebug("BrowserMsgRouter: Fixed element #", id, " was added.");

			// Tell Renderer to read out bounding rectangle coordinates belonging to the given ID
			CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("FetchFixedElements");
			msg->GetArgumentList()->SetInt(0, atoi(id.c_str()));
			browser->SendProcessMessage(PID_RENDERER, msg);

			return true;
		}
		
	}

	// Print message to console and withdraw callback
	LogDebug("MsgHandler: ", requestName);
	callback->Failure(0, "");

	return false;
}