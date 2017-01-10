//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Mueller (muellerd@uni-koblenz.de)
//============================================================================

#ifndef CEF_BROWSERMSGROUTER_H_
#define CEF_BROWSERMSGROUTER_H_

#include "src/Utils/Helper.h"
#include "include/wrapper/cef_message_router.h"
#include <functional>

class Mediator;		// Forward declaration
class BrowserMsgRouter; // Forward declaration

// Default handler for messages
class DefaultMsgHandler :	public CefMessageRouterBrowserSide::Handler
{
public:

	// Constructor
	DefaultMsgHandler(Mediator* pMediator) { _pMediator = pMediator; }

	// Called when |cefQuery| was called in Javascript
	virtual bool OnQuery(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		int64 query_id,
		const CefString& request,
		bool persistent,
		CefRefPtr<Callback> callback) OVERRIDE;

private:

	// Pointer to mediator (TODO: some extra interface?)
	Mediator* _pMediator;
};

// Callback handler
class CallbackMsgHandler : public CefMessageRouterBrowserSide::Handler
{
public:

	// Constructor
	CallbackMsgHandler(std::string prefix, std::function<void(std::string)> callbackFunction)
	{
		_prefix = prefix;
		_callbackFunction = callbackFunction;
	}

	// Called when |cefQuery| was called in Javascript
	virtual bool OnQuery(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		int64 query_id,
		const CefString& request,
		bool persistent,
		CefRefPtr<Callback> callback) OVERRIDE;

private:

	// Members
	std::string _prefix;
	std::function<void(std::string)> _callbackFunction;

};

// Router that owns multiple handlers of messages
class BrowserMsgRouter : public CefBase
{
public:
	BrowserMsgRouter(Mediator* pMediator);

	// Redirecting all method calls to browser side message router
	void OnBeforeBrowse(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame) const
	{
		_router->OnBeforeBrowse(browser, frame);
	}

	void OnBeforeClose(CefRefPtr<CefBrowser> browser) const
	{ 
		_router->OnBeforeClose(browser); 
	}

	bool OnProcessMessageReceived(
		CefRefPtr<CefBrowser> browser,
		CefProcessId source_process,
		CefRefPtr<CefProcessMessage> message) const
	{
		return _router->OnProcessMessageReceived(browser, source_process, message);
	}

	void OnRenderProcessTerminated(CefRefPtr<CefBrowser> browser) const
	{
		_router->OnRenderProcessTerminated(browser);
	}

	Mediator* GetMediator() const { return _pMediator; };

	// Create new handler to process this callback
	void RegisterJavascriptCallback(std::string prefix, std::function<void (std::string)> callbackFunction)
	{
		CefMessageRouterBrowserSide::Handler* callbackHandler = new CallbackMsgHandler(prefix, callbackFunction);
		_router->AddHandler(callbackHandler, false);
	}

private:

	// Wrapping CEF browser side message router
	CefRefPtr<CefMessageRouterBrowserSide> _router; // holds multiple handlers

	// Keep reference to Handler class in order to communicate with whole CEF architecture
	Mediator* _pMediator;

	IMPLEMENT_REFCOUNTING(BrowserMsgRouter);
};



#endif // CEF_BROWSERMSGROUTER_H_