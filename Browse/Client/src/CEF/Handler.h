//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Müller (muellerd@uni-koblenz.de)
//============================================================================

#ifndef CEF_HANDLER_H_
#define CEF_HANDLER_H_

#include "src/CEF/Renderer.h"
#include "src/CEF/Extension/JSCode.h"
#include "include/cef_client.h"
#include "src/CEF/BrowserMsgRouter.h"
#include <list>
#include <set>

// Forward declaration
class CefMediator;
class BrowserMsgRouter;

class Handler : public CefClient,
                public CefDisplayHandler,
                public CefLifeSpanHandler,
                public CefLoadHandler
{
public:

    Handler(CefMediator* pMediator, CefRefPtr<Renderer> renderer);
    ~Handler();

    // Provide access to the single global instance of this object
    static Handler* GetInstance();

    // Request that all existing browser windows close
    void CloseAllBrowsers(bool forceClose);
    bool IsClosing() const { return _isClosing; }

    // CefClient methods
    virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() OVERRIDE { return this; }
    virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() OVERRIDE { return this; }
    virtual CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE { return this; }
    virtual CefRefPtr<CefRenderHandler> GetRenderHandler() OVERRIDE { return _renderer; }

    // Life span handling of CefBrowsers
    virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
    virtual bool DoClose(CefRefPtr<CefBrowser> browser) OVERRIDE;
    virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) OVERRIDE;

    // Loading state callbacks
    virtual void OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame) OVERRIDE;
    virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode) OVERRIDE;
    virtual void OnLoadingStateChange(CefRefPtr<CefBrowser> browser,
        bool isLoading,
        bool canGoBack,
        bool canGoForward) OVERRIDE;
    virtual void OnLoadError(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        ErrorCode errorCode,
        const CefString& errorText,
        const CefString& failedUrl) OVERRIDE;

    // Only called by CefMediator
    void CloseBrowser(CefRefPtr<CefBrowser> browser);

    // Called when message from another process is received (e.g. IPC response from renderer process)
    virtual bool OnProcessMessageReceived(
        CefRefPtr<CefBrowser> browser,
        CefProcessId source_process,
        CefRefPtr<CefProcessMessage> message) OVERRIDE;

    // Load URL in specific browser
    void LoadPage(CefRefPtr<CefBrowser> browser, std::string url);

    // Page navigation
    void Reload(CefRefPtr<CefBrowser> browser);
    void GoBack(CefRefPtr<CefBrowser> browser);
    void GoForward(CefRefPtr<CefBrowser> browser);

    // Called by CefMediator, when window resize happens
    void ResizeBrowsers();

    // Emulation of mouse buttons in specific browser
    void EmulateLeftMouseButtonClick(CefRefPtr<CefBrowser> browser, double x, double y);
    void EmulateMouseWheelScrolling(CefRefPtr<CefBrowser> browser, double deltaX, double deltaY);

    // Inject text input to given DOM node (via Javascript)
    bool InputTextData(CefRefPtr<CefBrowser> browser, int64 frameID, int nodeID, std::string text, bool submit = false);

    void ResetMainFramesScrolling(CefRefPtr<CefBrowser> browser);

    // Bool value indicates need to reload DOM node data, true when called from outside of Tab due to changes
    void SetZoomLevel(CefRefPtr<CefBrowser> browser, bool definitelyChanged = true);

    // Write page resolution to V8 variables, read them and update Tab
    void UpdatePageResolution(CefRefPtr<CefBrowser> browser);

    // EXPERIMENTAL: Request coordinates of fixed elements (like bars on top of pages)
    void GetFixedElements(CefRefPtr<CefBrowser> browser);

	// CefDisplayHandler callbacks
	void OnFaviconURLChange(CefRefPtr<CefBrowser> browser,
		const std::vector<CefString>& icon_urls) OVERRIDE;

private:

    /* METHODS */

    // Log messages from renderer process on receiving logging relevant IPC messages
    void IPCLogRenderer(CefRefPtr<CefBrowser> browser, CefRefPtr<CefProcessMessage> msg);

    // Send request to renderer process in order to update DOM node information
    void ReloadDOMNodes(CefRefPtr<CefBrowser> browser, std::string debug_info = "");

    /* MEMBERS */

    // Used for an early DOM node update, if main frame loading takes too long because of multiple subframes
    std::map<int64, bool> _loadingMainFrames;

    // List of existing browser windows. Only accessed on the CEF UI thread
    typedef std::list<CefRefPtr<CefBrowser> > BrowserList;
    BrowserList _browserList;
    // Handler's current closing state
    bool _isClosing;

    // Provide pointer to CefMediator in order to communicate with Tabs etc.
    CefMediator* _pMediator;

    // Renderer, whose methods are called when rendering relevant actions take place
    CefRefPtr<Renderer> _renderer;

	// Message router for Javascript induced C++ callbacks
	CefRefPtr<BrowserMsgRouter> _msgRouter;

    // Javascript code as Strings
    const std::string _js_remove_css_scrollbar = GetJSCode(REMOVE_CSS_SCROLLBAR);
    const std::string _js_fixed_element_search = GetJSCode(FIXED_ELEMENT_SEARCH);

	// Set for parsing strings (as char by accessing it with []) to numbers
	std::set<char> digits = { '0', '1', '2', '3', '4', '5', '6' ,'7', '8', '9' };

    // Include CEF'S default reference counting implementation
    IMPLEMENT_REFCOUNTING(Handler);
};

#endif  // CEF_HANDLER_H_
