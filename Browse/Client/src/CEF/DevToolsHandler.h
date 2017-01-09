//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Mueller (muellerd@uni-koblenz.de)
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Handler for developer tools. Displayed in separate window.

#ifndef CEF_DEVTOOLSHANDLER_H_
#define CEF_DEVTOOLSHANDLER_H_

#include "include/cef_client.h"
#include <list>

class DevToolsHandler :	public CefClient,
						public CefDisplayHandler,
						public CefLifeSpanHandler,
						public CefLoadHandler 
{
public:

	// CefClient methods
	virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() OVERRIDE {	return this; }
	virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() OVERRIDE { return this; }
	virtual CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE { return this; }

	// CefDisplayHandler methods
	virtual void OnTitleChange(
		CefRefPtr<CefBrowser> browser,
		const CefString& title) OVERRIDE;

	// CefLifeSpanHandler methods
	virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
	virtual bool DoClose(CefRefPtr<CefBrowser> browser) OVERRIDE;
	virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) OVERRIDE;

	// CefLoadHandler methods
	virtual void OnLoadError(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		ErrorCode errorCode,
		const CefString& errorText,
		const CefString& failedUrl) OVERRIDE;

	// Request that all existing browser windows to close
	void CloseAllBrowsers(bool forceClose);
	bool IsClosing() const { return _isClosing; }

private:

	// Members
	typedef std::list<CefRefPtr<CefBrowser> > BrowserList;
	BrowserList _browserList;
	bool _isClosing = false;

	// Include CEF'S default reference counting implementation
	IMPLEMENT_REFCOUNTING(DevToolsHandler);
};

#endif // CEF_DEVTOOLSHANDLER_H_
