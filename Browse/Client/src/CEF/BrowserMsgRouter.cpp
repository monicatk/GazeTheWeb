//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Müller (muellerd@uni-koblenz.de)
//============================================================================

#include "BrowserMsgRouter.h"
#include "src/CEF/Extension/CefMediator.h"
#include "src/Utils/Logger.h"

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

	// Print message to console and withdraw callback
	LogDebug("MsgHandler: ", requestName);
	callback->Failure(0, "");

	return false;
}