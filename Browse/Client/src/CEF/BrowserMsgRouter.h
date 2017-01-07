//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Müller (muellerd@uni-koblenz.de)
//============================================================================

#ifndef CEF_BROWSERMSGROUTER_H_
#define CEF_BROWSERMSGROUTER_H_

#include "include/wrapper/cef_message_router.h"
#include <functional>

class Mediator;		// Forward declaration
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

	void RegisterJavascriptCallback(std::string prefix, std::function<void (std::string)>& callbackFunction)
	{
		// Add tupel of string prefix and function adress, which is going to be called when prefix is found, to map
		_externalCallbacks.emplace(prefix, callbackFunction);
	}

private:
	// Keep reference to msg router to handle outgoing commands
	CefRefPtr<BrowserMsgRouter> _pMsgRouter;

	std::map<std::string, std::function<void (std::string)>&> _externalCallbacks;

	void SearchForExternalCallbacks(std::string request)
	{
		for (const auto& tupel : _externalCallbacks)
		{
			const auto& prefix = tupel.first;
			// Check if first letters in request equal given prefix
			if (request.substr(0, prefix.size()).compare(request) == 0)
			{
				// Execute external callback function
				(tupel.second)(request);
				// Stop searching in list of prefixes
				break;
			}
		}
	}

	//IMPLEMENT_REFCOUNTING(MsgHandler);
};



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

	void RegisterJavascriptCallback(std::string prefix, std::function<void (std::string)>& callbackFunction)
	{

	}

private:
	// Wrapping CEF browser side message router
	CefRefPtr<CefMessageRouterBrowserSide> _router;

	// Keep reference to Handler class in order to communicate with whole CEF architecture
	Mediator* _pMediator;

	IMPLEMENT_REFCOUNTING(BrowserMsgRouter);
};



#endif // CEF_BROWSERMSGROUTER_H_