//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Müller (muellerd@uni-koblenz.de)
//============================================================================

#ifndef CEF_BROWSERMSGROUTER_H_
#define CEF_BROWSERMSGROUTER_H_

#include "include/wrapper/cef_message_router.h"
#include <functional>

class CefMediator;		// Forward declaration
class BrowserMsgRouter; // Forward declaration

class MsgHandler :	public CefMessageRouterBrowserSide::Handler
					//public CefBase
{
public:

	MsgHandler(BrowserMsgRouter* pMsgRouter);

	// Called when |cefQuery| was called in Javascript
	virtual bool OnQuery(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		int64 query_id,
		const CefString& request,
		bool persistent,
		CefRefPtr<Callback> callback) OVERRIDE;

	std::vector<std::string> SplitBySeparator(std::string str, char separator);

private:
	// Keep reference to msg router to handle outgoing commands
	CefRefPtr<BrowserMsgRouter> _pMsgRouter;

	//IMPLEMENT_REFCOUNTING(MsgHandler);
};



class BrowserMsgRouter : public CefBase
{
public:
	BrowserMsgRouter(CefMediator* pMediator);

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

	CefMediator* GetMediator() const { return _pMediator; };

private:
	// Wrapping CEF browser side message router
	CefRefPtr<CefMessageRouterBrowserSide> _router;

	// Keep reference to Handler class in order to communicate with whole CEF architecture
	CefMediator* _pMediator;

	IMPLEMENT_REFCOUNTING(BrowserMsgRouter);
};



#endif // CEF_BROWSERMSGROUTER_H_