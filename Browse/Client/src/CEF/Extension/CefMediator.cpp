//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Müller (muellerd@uni-koblenz.de)
//============================================================================

#include "CefMediator.h"
#include "src/CEF/Extension/JSCode.h"
#include "src/CEF/Extension/Container.h"
#include "src/Setup.h"
#include "src/State/Web/Tab/Interface/TabCEFInterface.h"
#include "src/CEF/Data/DOMNode.h"
#include "src/Utils/Logger.h"
#include "include/cef_app.h"
#include "include/wrapper/cef_helpers.h"
#include "src/CEF/DevToolsHandler.h"


void CefMediator::RegisterTab(TabCEFInterface* pTab)
{
    CEF_REQUIRE_UI_THREAD();

    /*Following information might be changed and used in the future
    http://magpcss.org/ceforum/apidocs3/projects/%28default%29/_cef_browser_settings_t.html
        - webgl: enable WebGL
        - windowless_frame_rate: default 30 fps, maximum 60
    */
    CefWindowInfo window_info;				// CefBrowser relevant information
    window_info.SetAsWindowless(0, false);	// Window handle set to zero (may cause visual errors)
    CefBrowserSettings browser_settings;	// Browser settings

    // Enable WebGL part 1 (other is in App.cpp)
    if(setup::ENABLE_WEBGL)
    {
        browser_settings.webgl = STATE_ENABLED;
        browser_settings.windowless_frame_rate = 60;
	
    }

	browser_settings.application_cache = STATE_ENABLED; // Goal: Accepting & saving cookies enabled

    // TODO: Delay Renderer OR save Tab & Texture as default if key(browser) not in map, reset after map insertion
    //			OR add maps to CreateBrowserSync and fill them before Renderer starts (if possible)

    _pendingTab = pTab;

    LogDebug("CefMediator: Creating new CefBrowser at Tab registration.");
    // Create new CefBrowser with given information
    CefRefPtr<CefBrowser> browser = CefBrowserHost::CreateBrowserSync(
        window_info, _handler.get(), pTab->GetURL(), browser_settings, NULL);

    // Fill maps with correlating Tab and CefBrowsre
    _browsers.emplace(pTab, browser);
    _tabs.emplace(browser->GetIdentifier(), pTab);
    _pendingTab = NULL;
}

void CefMediator::UnregisterTab(TabCEFInterface* pTab)
{

    if (CefRefPtr<CefBrowser> browser = GetBrowser(pTab))
    {
        // Close corresponding CefBrowser in Handler
        _handler->CloseBrowser(browser);

        // Delete corresponding key-value-pair
        BrowserID browserID = browser->GetIdentifier();
        LogDebug("CefMediator: Unregistering Tab corresponding to browserID = ", browserID);
        _tabs.erase(browserID);
        _browsers.erase(pTab);
    }
}

void CefMediator::RefreshTab(TabCEFInterface * pTab)
{
    if (CefRefPtr<CefBrowser> browser = GetBrowser(pTab))
    {
        // Get Tab object and load Tab's URL in CefBrowser
        _handler->LoadPage(_browsers.at(pTab), pTab->GetURL());
    }
}

void CefMediator::ReloadTab(TabCEFInterface * pTab)
{
    if (CefRefPtr<CefBrowser> browser = GetBrowser(pTab))
    {
        _handler->Reload(browser);
    }
}

void CefMediator::GoBack(TabCEFInterface * pTab)
{
    if (CefRefPtr<CefBrowser> browser = GetBrowser(pTab))
    {
        _handler->GoBack(browser);
    }
}

void CefMediator::GoForward(TabCEFInterface * pTab)
{
    if (CefRefPtr<CefBrowser> browser = GetBrowser(pTab))
    {
        _handler->GoForward(browser);
    }
}

void CefMediator::OpenNewTab(std::string url)
{
    // TODO? When is it called (asks Raphael)
	// Daniel: Seems as if it was planned but then obsolete.. whoops. :D
}

void CefMediator::DoMessageLoopWork()
{
    CefDoMessageLoopWork();
}

void CefMediator::EmulateMouseCursor(TabCEFInterface* pTab, double x, double y, bool leftButtonPressed)
{
    if(CefRefPtr<CefBrowser> browser = GetBrowser(pTab))
    {
        _handler->EmulateMouseCursor(browser, x, y, leftButtonPressed);
    }
}

void CefMediator::EmulateLeftMouseButtonClick(TabCEFInterface * pTab, double x, double y)
{
    if(CefRefPtr<CefBrowser> browser = GetBrowser(pTab))
    {
        LogDebug("CefMediator: Emulating left mouse button click on position (x, y) = (", x, ", ", y, ").");

        // Get Tab information and do browser work in Handler
        _handler->EmulateLeftMouseButtonClick(browser, x, y);
    }
}

void CefMediator::EmulateMouseWheelScrolling(TabCEFInterface * pTab, double deltaX, double deltaY)
{
    if (CefRefPtr<CefBrowser> browser = GetBrowser(pTab))
    {
        _handler->EmulateMouseWheelScrolling(browser, deltaX, deltaY);
    }
}

void CefMediator::ReceiveIPCMessageforFavIcon(CefRefPtr<CefBrowser> browser, CefRefPtr<CefProcessMessage> msg)
{
    const std::string& msgName = msg->GetName().ToString();

    if (msgName == "ReceiveFavIconBytes")
    {
		LogDebug("CefMediator: Received Favicon Bytes...");

        CefRefPtr<CefListValue> args = msg->GetArgumentList();
        int index = 0;
        if(TabCEFInterface* pTab = GetTab(browser))
        {
            const int width = args->GetInt(index++);
            const int height = args->GetInt(index++);

            if (width <= 0 || height <= 0)
            {
                LogDebug("CefMediator: Failure. Received favicon width or height less than zero!");

                if (TabCEFInterface* pInnerTab = GetTab(browser))
                {
                    // Inform Tab anyway
                    pInnerTab->ReceiveFaviconBytes(nullptr, width, height);
                }
            }
            else
            {
                LogDebug("CefMediator: Success. Copying favicon bytes to Tab... (w= ", width, ", h=", height,")");

                // Extract width, height and byte information
                std::unique_ptr<std::vector<unsigned char> > upData = std::unique_ptr<std::vector<unsigned char> >(new std::vector<unsigned char>);
                for (int i = 0; i < height*width; i++)
                {
                    int four_bytes = args->GetInt(index++);

                    for (int j = 3; j >= 0; j--)
                    {
                        // Read byte for each channel (RGBA) from 4 byte integer value
                        upData->push_back((four_bytes >> (j*8)) & 0xFF);
                    }
                }

                // Pipe extracted information to Tab
                pTab->ReceiveFaviconBytes(std::move(upData), width, height);

                // Save new favicon URL in Tab
                //pTab->SetFavIconURL(iconUrl);	// TODO: With favicon callback, saving favicon URL in Tab really neccessary? I don't think so.
            }

        }
    }
}

void CefMediator::ResetFavicon(CefRefPtr<CefBrowser> browser)
{
    if (TabCEFInterface* pTab = GetTab(browser))
    {
        pTab->ResetFaviconBytes();
    }
}

void CefMediator::AddDOMNode(CefRefPtr<CefBrowser> browser, std::shared_ptr<DOMNode> spNode)
{
    if (TabCEFInterface* pTab = GetTab(browser))
    {
        // Find corresponding Tab and add DOM Node to its list of nodes
        pTab->AddDOMNode(spNode);
    }
}

void CefMediator::ClearDOMNodes(CefRefPtr<CefBrowser> browser)
{
    if (TabCEFInterface* pTab = GetTab(browser))
    {
        // Clear corresponding Tabs DOM Node list (implicitly destroy all DOM Node objects)
        pTab->ClearDOMNodes();
		//LogDebug("CefMediator: ### DISABLED DOM CLEARING FOR LINK TESTING ###");
        LogDebug("CefMediator: Cleared all DOM nodes belonging to browserID = ", browser->GetIdentifier());
    }
}

bool CefMediator::InputTextData(TabCEFInterface* tab, int64 frameID, int nodeID, std::string text, bool submit)
{
    if (CefRefPtr<CefBrowser> browser = GetBrowser(tab))
    {
        LogDebug("CefMediator: Input text '", text, "' in frame id = ", frameID, " (nodeID = ", nodeID, ").");
        return _handler->InputTextData(browser, frameID, nodeID, text, submit);
    }
    return false;
}

void CefMediator::FillDOMNodeWithData(CefRefPtr<CefBrowser> browser, CefRefPtr<CefProcessMessage> msg)
{
	if (TabCEFInterface* pTab = GetTab(browser))
	{
		int index = 0;
		CefRefPtr<CefListValue> args = msg->GetArgumentList();
		int type = args->GetInt(index++);
		int nodeID = args->GetInt(index++);
		bool visible = args->GetBool(index++);

		// TODO(in the future): Call an update of different attributes for different node types

		// Read out multiple Rects
		std::vector<Rect> rects;

		for (int i = index; i + 3 < args->GetSize(); i += 4)
		{
			rects.push_back(
				Rect(
					args->GetDouble(i),
					args->GetDouble(i + 1),
					args->GetDouble(i + 2),
					args->GetDouble(i + 3)
					)
				);
		}

		// Receive weak_ptr to target node and use it as shared_ptr targetNode
		if (auto targetNode = pTab->GetDOMNode((DOMNodeType) type, nodeID).lock())
		{
			// Update target nodes Rects
			targetNode->SetRects(std::make_shared<std::vector<Rect>>(rects));

			// Set target node's visibility
			targetNode->SetVisibility(visible);
			//if (!visible) LogDebug("CefMediator: Set node's visibility to false after its creation");
		}
		else
		{
			LogDebug("CefMediator: Trying to update node information but DOMNode object doesn't seem to exist!");
		}
	}
}

void CefMediator::SetTabActive(TabCEFInterface * pTab)
{
	// Remember currently active Tab
	_activeTab = pTab;

	for (const auto& key : _browsers)
	{
		CefRefPtr<CefBrowser> browser = key.second;
		if (key.first == pTab)
		{
			// Activate the given Tab's rendering
			browser->GetHost()->WasHidden(false);
		}
		else
		{
			// Deactivate rendering of all other Tabs
			browser->GetHost()->WasHidden(true);
		}

	}
}



bool CefMediator::SetLoadingStatus(CefRefPtr<CefBrowser> browser, int64 frameID, bool isMain, bool isLoading)
{
	if (TabCEFInterface* pTab = GetTab(browser))
	{
		pTab->SetLoadingStatus(frameID, isMain, isLoading);
		return true;
	}
	else
	{
		return false;
	}
	
}

void CefMediator::ScrollOverflowElement(TabCEFInterface * pTab, int elemId, int x, int y)
{
	if (CefRefPtr<CefBrowser> browser = GetBrowser(pTab))
	{
		_handler->ScrollOverflowElement(browser, elemId, x, y);
	}
}

void CefMediator::AddOverflowElement(CefRefPtr<CefBrowser> browser, std::shared_ptr<OverflowElement> overflowElem)
{
	if (TabCEFInterface* pTab = GetTab(browser))
	{
		pTab->AddOverflowElement(overflowElem);
	}
}

std::weak_ptr<OverflowElement> CefMediator::GetOverflowElement(CefRefPtr<CefBrowser> browser, int id)
{
	if (TabCEFInterface* pTab = GetTab(browser))
	{
		return pTab->GetOverflowElement(id);
	}
	return std::weak_ptr<OverflowElement>();
}

void CefMediator::RemoveOverflowElement(CefRefPtr<CefBrowser> browser, int id)
{
	if (TabCEFInterface* pTab = GetTab(browser))
	{
		pTab->RemoveOverflowElement(id);
	}
}

void CefMediator::ResetScrolling(TabCEFInterface * pTab)
{
    if (CefRefPtr<CefBrowser> browser = GetBrowser(pTab))
    {
        _handler->ResetMainFramesScrolling(browser);
    }
}

void CefMediator::SetURL(CefRefPtr<CefBrowser> browser)
{
    if (TabCEFInterface* pTab = GetTab(browser))
    {
        pTab->SetURL(browser->GetMainFrame()->GetURL());
    }
}

void CefMediator::SetCanGoBack(CefRefPtr<CefBrowser> browser, bool canGoBack)
{
    if (TabCEFInterface* pTab = GetTab(browser))
    {
        pTab->SetCanGoBack(canGoBack);
    }
}

void CefMediator::SetCanGoForward(CefRefPtr<CefBrowser> browser, bool canGoForward)
{
    if (TabCEFInterface* pTab = GetTab(browser))
    {
        pTab->SetCanGoForward(canGoForward);
    }
}

void CefMediator::SetZoomLevel(TabCEFInterface * pTab)
{
    if (CefRefPtr<CefBrowser> browser = GetBrowser(pTab))
    {
        _handler->SetZoomLevel(browser);
    }
}

double CefMediator::GetZoomLevel(CefRefPtr<CefBrowser> browser)
{
    if (TabCEFInterface* pTab = GetTab(browser))
    {
        return pTab->GetZoomLevel();
    }
    return NAN;
}

void CefMediator::ReceivePageResolution(CefRefPtr<CefBrowser> browser, CefRefPtr<CefProcessMessage> msg)
{
    if (TabCEFInterface* pTab = GetTab(browser))
    {
        CefRefPtr<CefListValue> args = msg->GetArgumentList();
        double pageWidth = args->GetDouble(0), pageHeight = args->GetDouble(1);
        pTab->SetPageResolution(pageWidth, pageHeight);
    }
}

void CefMediator::GetPageResolution(TabCEFInterface * pTab)
{
    if (CefRefPtr<CefBrowser> browser = GetBrowser(pTab))
    {
        _handler->UpdatePageResolution(browser);
    }
}

void CefMediator::ReceiveFixedElements(CefRefPtr<CefBrowser> browser, CefRefPtr<CefProcessMessage> msg)
{
    CefRefPtr<CefListValue> args = msg->GetArgumentList();
    int id = args->GetInt(0);
    std::vector<Rect> fixedCoords = {};
    for (int i = 1; i < (int)args->GetSize(); i+=4)
    {
        fixedCoords.push_back(
            Rect(
                args->GetDouble(i), // top
                args->GetDouble(i + 1), // left
                args->GetDouble(i + 2), // bottom
                args->GetDouble(i + 3) // right
                )
            );
    }
    if (TabCEFInterface* pTab = GetTab(browser))
    {
      /*  LogDebug("CefMediator: Sending ", fixedCoords.size(), " fixed element coordinate tupel(s) to Tab for fixedID=", id, ".");
        for (int i = 0; i < (int)fixedCoords.size(); i++)
        {
             LogDebug("\t-->", fixedCoords[i].top, ", ", fixedCoords[i].left, ", ", fixedCoords[i].bottom, ", ", fixedCoords[i].right);
        }*/
        pTab->AddFixedElementsCoordinates(id, fixedCoords);
    }
}

void CefMediator::RemoveFixedElement(CefRefPtr<CefBrowser> browser, int id)
{
    if (TabCEFInterface* pTab = GetTab(browser))
    {
        //LogDebug("CefMediator: Removing fixed element with id=", id, " from Tab.");
        pTab->RemoveFixedElement(id);
    }
}

void CefMediator::Poll(float tpf)
{
    // TODO Daniel
}

void CefMediator::OnTabTitleChange(CefRefPtr<CefBrowser> browser, std::string title)
{
	if (TabCEFInterface* pTab = GetTab(browser))
	{
		pTab->SetTitle(title);
	}
}

void CefMediator::OpenPopupTab(CefRefPtr<CefBrowser> browser, std::string url)
{
	if (TabCEFInterface* pTab = GetTab(browser))
	{
        pTab->AddTabAfter(url);
	}
}


void CefMediator::RemoveDOMNode(CefRefPtr<CefBrowser> browser, DOMNodeType type, int nodeID)
{
	if (TabCEFInterface* pTab = GetTab(browser))
	{
		pTab->RemoveDOMNode(type, nodeID);
	}
}

std::weak_ptr<DOMNode> CefMediator::GetDOMNode(CefRefPtr<CefBrowser> browser, DOMNodeType type, int nodeID)
{
	if (TabCEFInterface* pTab = GetTab(browser))
	{
		return pTab->GetDOMNode(type, nodeID);
	}
	return std::weak_ptr<DOMNode>();
}


void CefMediator::ShowDevTools()
{
	LogDebug("CefMediator: Showing DevTools...");

	CefRefPtr<SimpleHandler> devToolHandler(new SimpleHandler());
	CefWindowInfo window_info;

#if defined(OS_WIN)
	// On Windows we need to specify certain flags that will be passed to
	// CreateWindowEx().
	window_info.SetAsPopup(NULL, "DevTools");
#endif
	CefBrowserSettings settings;

	if (_activeTab)
	{
		auto browser = GetBrowser(_activeTab);
		if (browser)
		{
			browser->GetHost()->ShowDevTools(window_info, devToolHandler.get(), settings, CefPoint());
		}
	}
	// TODO: Delete "else" path with for-each loop when SetTabActive is integrated & called
	else
	{
		for (const auto& key : _browsers)
		{

			CefRefPtr<CefBrowser> browser = key.second;
			browser->GetHost()->ShowDevTools(window_info, devToolHandler.get(), settings, CefPoint());
		}
	}

}

void CefMediator::EmulateLeftMouseButtonDown(TabCEFInterface* pTab, double x, double y)
{
	if (const CefRefPtr<CefBrowser> browser = GetBrowser(pTab))
	{
		// Create mouse event
		CefMouseEvent event;
		event.x = x;
		event.y = y;

		// Send mouse down event to tab
		browser->GetHost()->SendMouseClickEvent(event, MBT_LEFT, false, 1);
	}
}

void CefMediator::EmulateLeftMouseButtonUp(TabCEFInterface* pTab, double x, double y)
{
	if (const CefRefPtr<CefBrowser> browser = GetBrowser(pTab))
	{
		// Create mouse event
		CefMouseEvent event;
		event.x = x;
		event.y = y;

		// Send mouse up event to tab
		browser->GetHost()->SendMouseClickEvent(event, MBT_LEFT, true, 1);

		// Call JS GetTextSelection and receive selected text as string in MsgRouter
		// TODO: maybe separate call?
		// browser->GetMainFrame()->ExecuteJavaScript("GetTextSelection();", "", 0);
	}
}

void CefMediator::InvokeCopy(TabCEFInterface * pTab)
{
	LogDebug("CefMediator: InvokeCopy called.");

	// if (const auto& browser = GetBrowser(pTab))
	for(const auto& key : _browsers) // TODO: Use pTab instead
	{
		const auto& browser = key.second;

		browser->GetFocusedFrame()->Copy();
	}
}

void CefMediator::InvokePaste(TabCEFInterface * pTab, double x, double y)
{
	LogDebug("CefMediator: InvokePaste called on position (", x, ", ", y, ").");

	// if (const auto& browser = GetBrowser(pTab))
	for (const auto& key : _browsers)	// TODO: Use pTab instead
	{
		const auto& browser = key.second;

		CefMouseEvent event;
		event.x = x;
		event.y = y;
		// Click at position where text should be pasted
		browser->GetHost()->SendMouseClickEvent(event, MBT_LEFT, false, 1);

		browser->GetFocusedFrame()->Paste();
	}
}



TabCEFInterface* CefMediator::GetTab(CefRefPtr<CefBrowser> browser) const
{
    int browserID = browser->GetIdentifier();
    if (_tabs.find(browserID) != _tabs.end())
    {
        return _tabs.at(browserID);
    }
    LogDebug("CefMediator: The given CefBrowser pointer is not contained in key in Browser->Tab map (anymore).");
    return nullptr;
}

CefRefPtr<CefBrowser> CefMediator::GetBrowser(TabCEFInterface * pTab) const
{
    if (_browsers.find(pTab) != _browsers.end())
    {
        return _browsers.at(pTab);
    }
    LogDebug("CefMediator: The given Tab pointer is not contained in key in Tab->CefBrowser map (anymore).");
    return nullptr;
}


std::weak_ptr<Texture> CefMediator::GetTexture(CefRefPtr<CefBrowser> browser)
{
    if (TabCEFInterface* pTab = GetTab(browser))
    {
        return pTab->GetWebViewTexture();
    }
    else if (_pendingTab)
    {
        LogDebug("CefMediator: Pending Tab used for GetTexture.");
        return _pendingTab->GetWebViewTexture();
    }
    else
    {
        LogDebug("CefMediator: GetTexture did not find corresponding texture.");
        // Return empty weak pointer
        return std::weak_ptr<Texture>();
    }
}

void CefMediator::OnScrollOffsetChanged(CefRefPtr<CefBrowser> browser, double x, double y)
{
    if (TabCEFInterface* pTab = GetTab(browser))
    {
        // Set scrolling offset of correlating Tab
        pTab->SetScrollingOffset(x, y);
    }
}

void CefMediator::GetResolution(CefRefPtr<CefBrowser> browser, int& width, int& height) const
{
    if (TabCEFInterface* pTab = GetTab(browser))
    {
        pTab->GetWebRenderResolution(width, height);
    }
    else if (_pendingTab)
    {
        LogDebug("CefMediator: Getting resolution from Tab _pendingTab.");
        _pendingTab->GetWebRenderResolution(width, height);
    }
    else
    {
        width = 0;
        height = 0;
        LogDebug("CefMediator: GetResolution returned (0,0).");
    }
}

void CefMediator::ResizeTabs()
{
    _handler->ResizeBrowsers();
}
