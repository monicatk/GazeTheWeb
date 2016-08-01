//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Müller (muellerd@uni-koblenz.de)
//============================================================================

#ifndef CEF_MEDIATOR_H_
#define CEF_MEDIATOR_H_

#include "src/CEF/Handler.h"
#include "include/cef_browser.h"
#include "include/cef_base.h"
#include <map>
#include <memory>
#include <queue>

/**
*	Expand CefApp by methods and attributes used to communicate with Master and
*   Tab classes.
*
*/

// Forward declarations
class TabCEFInterface;
class Texture;
class DOMNode;
enum DOMNodeType;

typedef int BrowserID;

class CefMediator : public CefBase
{
public:

    // Receive tab specific commands
    void RegisterTab(TabCEFInterface* pTab);
    void UnregisterTab(TabCEFInterface* pClosing);
    void RefreshTab(TabCEFInterface* pTab);
    void ReloadTab(TabCEFInterface* pTab);
    void GoBack(TabCEFInterface* pTab);
    void GoForward(TabCEFInterface* pTab);
	void OpenNewTab(std::string url);

    // Renderer::OnPaint calls this method in order to receive corresponding Texture
    std::weak_ptr<Texture> GetTexture(CefRefPtr<CefBrowser> browser);

    // Equally named Renderer's callback pipes data to CefMediator, who sets offset in correlating tab
    void OnScrollOffsetChanged(CefRefPtr<CefBrowser> browser, double x, double y);

    // Get resolution of rendering
    void GetResolution(CefRefPtr<CefBrowser> browser, int& width, int& height) const;

    // Called by Master when window resize happens
    void ResizeTabs();

    // Call from Master to do message loop work
    void DoMessageLoopWork();

    // Emulation of left mouse button press and release in specific Tab
    void EmulateLeftMouseButtonClick(TabCEFInterface* pTab, double x, double y);
    void EmulateMouseWheelScrolling(TabCEFInterface* pTab, double deltaX, double deltaY);

    void ResetScrolling(TabCEFInterface* pTab);

    // Sets Tab's URL attribute, called by Handler when main frame starts loading a page
    void SetURL(CefRefPtr<CefBrowser> browser);

    void ReceiveIPCMessageforFavIcon(CefRefPtr<CefBrowser> browser, CefRefPtr<CefProcessMessage> msg);
    void ResetFavicon(CefRefPtr<CefBrowser> browser);

    void SetCanGoBack(CefRefPtr<CefBrowser> browser, bool canGoBack);
    void SetCanGoForward(CefRefPtr<CefBrowser> browser, bool canGoForward);

    // External zoom level request
    void SetZoomLevel(TabCEFInterface* pTab);

    // Called by Handler OnLoadStart
    double GetZoomLevel(CefRefPtr<CefBrowser> browser);

    void ReceivePageResolution(CefRefPtr<CefBrowser> browser, CefRefPtr<CefProcessMessage> msg);

    // Called when Tab realizes that it might reach end of page while scrolling
    void GetPageResolution(TabCEFInterface* pTab);

    void ReceiveFixedElements(CefRefPtr<CefBrowser> browser, CefRefPtr<CefProcessMessage> msg);
    void RemoveFixedElement(CefRefPtr<CefBrowser> browser, int id);

    // Called by master, only. Quite similar to a update method
    void Poll(float tpf);

	// Update Tab's title when title change callback is received
	void OnTabTitleChange(CefRefPtr<CefBrowser> browser, std::string title);

	// Add new Tab with given URL at the position after the current Tab (in context of Tab overview)
	void OpenPopupTab(CefRefPtr<CefBrowser> browser, std::string url);



	// DOM relevant methods
	void ReceiveIPCMessageforDOM(CefRefPtr<CefBrowser> browser, CefRefPtr<CefProcessMessage> msg);

	void AddDOMNode(CefRefPtr<CefBrowser> browser, std::shared_ptr<DOMNode> spNode);
	void ClearDOMNodes(CefRefPtr<CefBrowser> browser);

	bool InputTextData(TabCEFInterface* tab, int64 frameID, int nodeID, std::string text, bool submit = false);

	// Called by BrowserMsgRouter to pass update information to Tab
	void UpdateDOMNode(CefRefPtr<CefBrowser> browser, DOMNodeType type, int nodeID, int attr, void* data);

	void RemoveDOMNode(CefRefPtr<CefBrowser> browser, DOMNodeType type, int nodeID);

	void HandleDOMNodeIPCMsg(CefRefPtr<CefBrowser> browser, CefRefPtr<CefProcessMessage> msg);

protected:

    /* MEMBERS */
    CefRefPtr<Handler> _handler;

    // Save corresponding (Tab, CefBrowser)-pairs in two maps
    std::map<TabCEFInterface*, CefRefPtr<CefBrowser>> _browsers;
    std::map<BrowserID, TabCEFInterface*> _tabs;

    TabCEFInterface* _pendingTab = NULL; // Used in Tab registration progress (at first, Renderer works to fast for map access)

    // Use these methods for less coding overhead by checking if key exists in map
    TabCEFInterface* GetTab(CefRefPtr<CefBrowser> browser) const;
    CefRefPtr<CefBrowser> GetBrowser(TabCEFInterface* pTab) const;

    IMPLEMENT_REFCOUNTING(CefMediator);
};


#endif // CEF_MEDIATOR_H_
