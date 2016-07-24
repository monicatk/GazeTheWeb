//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Müller (muellerd@uni-koblenz.de)
//============================================================================

#include "CefMediator.h"
#include "src/CEF/Extension/JSCode.h"
#include "src/CEF/Extension/Container.h"
#include "src/Setup.h"
#include "src/State/Web/Tab/TabCEFInterface.h"
#include "src/State/Web/Tab/DOMNode.h"
#include "src/Utils/Logger.h"
#include "include/cef_app.h"
#include "include/wrapper/cef_helpers.h"

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
}

void CefMediator::DoMessageLoopWork()
{
    CefDoMessageLoopWork();
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

void CefMediator::ReceiveIPCMessageforDOM(CefRefPtr<CefBrowser> browser, CefRefPtr<CefProcessMessage> msg)
{

    const std::string& msgName = msg->GetName().ToString();

    if (msgName == "ReceiveDOMElements")
    {
        // Get Tab to which future DOM nodes belong
        if (TabCEFInterface* target = GetTab(browser))
        {
            // Define function, which gets read data from IPC msg and creates DOM nodes according to container scheme (see Container.h)
            auto createDOMTextInputs = [target]
                    (
                    std::vector<int> integers,
                    std::vector<double> doubles,
                    std::vector<std::string> strings,
                    int64 frameID,
                    int nodeID
                    ) -> void
            {
                // Define where attributes and their values are expected according to scheme
                Rect rect = Rect(doubles[0], doubles[1], doubles[2], doubles[3]);

                std::string value = strings[0];

                // LogDebug("CefMediator: Creating DOM node.");

                // Add DOM node to Tab
                target->AddDOMNode(std::make_shared<DOMTextInput>(DOMNodeType::TextInput, frameID, nodeID, rect, value));

            };

            IPC_Container ipcContainer(domNodeScheme);
            ipcContainer.GetObjectsFromIPC(
                msg,
                std::vector< std::function< void(std::vector<int>, std::vector<double>, std::vector<std::string>, int64, int)> >{createDOMTextInputs}
            );
        }
    }
}

void CefMediator::ReceiveIPCMessageforFavIcon(CefRefPtr<CefBrowser> browser, CefRefPtr<CefProcessMessage> msg)
{
    const std::string& msgName = msg->GetName().ToString();

    if (msgName == "ReceiveFavIconBytes")
    {
        CefRefPtr<CefListValue> args = msg->GetArgumentList();
        int index = 0;
        if(TabCEFInterface* pTab = GetTab(browser))
        {
            const int width = args->GetInt(index++);
            const int height = args->GetInt(index++);

            if (width <= 0 || height <= 0)
            {
                LogDebug("CefMediator: Received favicon width or height less than zero!");

                if (TabCEFInterface* pInnerTab = GetTab(browser))
                {
                    // Inform Tab anyway
                    pInnerTab->ReceiveFaviconBytes(nullptr, width, height);
                }
            }
            else
            {
                LogDebug("CefMediator: Copying favicon bytes to Tab... (w= ", width, ", h=", height,")");

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

void CefMediator::CreateDOMNode(CefRefPtr<CefBrowser> browser, std::shared_ptr<DOMNode> spNode)
{
    if (TabCEFInterface* pTab = GetTab(browser))
    {
        // find corresponding Tab and add DOM Node to its list of nodes
        pTab->AddDOMNode(spNode);
    }
}

void CefMediator::ClearDOMNodes(CefRefPtr<CefBrowser> browser)
{
    if (TabCEFInterface* pTab = GetTab(browser))
    {
        // clear corresponding Tabs DOM Node list (implicitly destroy all DOM Node objects)
        pTab->ClearDOMNodes();
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
        LogDebug("CefMediator: Sending ", fixedCoords.size(), " fixed element coordinate tupel(s) to Tab for fixedID=", id, ".");
        for (int i = 0; i < (int)fixedCoords.size(); i++)
        {
             LogDebug("\t-->", fixedCoords[i].top, ", ", fixedCoords[i].left, ", ", fixedCoords[i].bottom, ", ", fixedCoords[i].right);
        }
        pTab->AddFixedElementsCoordinates(id, fixedCoords);
    }
}

void CefMediator::RemoveFixedElement(CefRefPtr<CefBrowser> browser, int id)
{
    if (TabCEFInterface* pTab = GetTab(browser))
    {
        LogDebug("CefMediator: Removing fixed element with id=", id, " from Tab.");
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
