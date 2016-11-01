//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Müller (muellerd@uni-koblenz.de)
//============================================================================

#include "src/CEF/Handler.h"
#include "src/CEF/Extension/CefMediator.h"
#include "src/Utils/Logger.h"
#include "include/base/cef_bind.h"
#include "include/cef_app.h"
#include "include/wrapper/cef_closure_task.h"
#include "include/wrapper/cef_helpers.h"
#include <sstream>
#include <string>
#include <cmath>

namespace
{
    Handler* g_instance = NULL;
}  // namespace

Handler::Handler(CefMediator* pMediator, CefRefPtr<Renderer> renderer) : _isClosing(false)
{
  DCHECK(!g_instance);
  g_instance = this;
  _pMediator = pMediator;
  _renderer = renderer;
  _msgRouter = new BrowserMsgRouter(pMediator);
}

Handler::~Handler()
{
  g_instance = NULL;
}

// Static
Handler* Handler::GetInstance()
{
  return g_instance;
}

void Handler::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
  CEF_REQUIRE_UI_THREAD();

  // Add to the list of existing browsers.
  _browserList.push_back(browser);
  LogDebug("Handler: New CefBrowser with id = ", browser->GetIdentifier(), " created.");
}

bool Handler::DoClose(CefRefPtr<CefBrowser> browser)
{
  CEF_REQUIRE_UI_THREAD();

  // Remove browser from browser list
  BrowserList::const_iterator it = _browserList.begin();
  int browserID = browser->GetIdentifier();
  for (; it != _browserList.end(); ++it)
  {
      if (it->get()->GetIdentifier() == browserID)
      {
          _browserList.remove(browser);
          break;
      }
  }

  // Allow the close. For windowed browsers this will result in the OS close
  // event being sent.
  return false;
}

void Handler::OnBeforeClose(CefRefPtr<CefBrowser> browser)
{
  CEF_REQUIRE_UI_THREAD();

  _msgRouter->OnBeforeClose(browser);

   //Remove from the list of existing browsers.
  BrowserList::iterator bit = _browserList.begin();
  for (; bit != _browserList.end(); ++bit)
  {
    if ((*bit)->IsSame(browser))
    {
        _browserList.erase(bit);
        break;
    }
  }

  //if (_browserList.empty())
  //{
  //  // All browser windows have closed. Quit the application's message loop
  //  LogDebug << "Quitting CEF message loop after all browser have shut down.";
  //  CefQuitMessageLoop();

  //}
}

void Handler::OnLoadError(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    ErrorCode errorCode,
    const CefString& errorText,
    const CefString& failedUrl)
{
    CEF_REQUIRE_UI_THREAD();

    // Don't display an error for downloaded files.
    if (errorCode == ERR_ABORTED)
        return;

    // Display a load error message.
    std::stringstream ss;
    ss << "<html><body bgcolor=\"white\">"
        "<h2>Failed to load URL " << std::string(failedUrl) <<
        " with error " << std::string(errorText) << " (" << errorCode <<
        ").</h2></body></html>";
    frame->LoadString(ss.str(), failedUrl);
}

void Handler::OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame)
{
	// Set loading status of each in Tab to loading in order to display loading icon and status
	_pMediator->SetLoadingStatus(browser, frame->GetIdentifier(), frame->IsMain(), true);

    //CEF_REQUIRE_UI_THREAD();
	if(frame->IsMain())
		LogDebug("Handler: Started loading frame id = ", frame->GetIdentifier(), " (main = ", frame->IsMain(), "), browserID = ", browser->GetIdentifier());

    if (frame->IsMain())
    {
/*
		browser->GetMainFrame()->ExecuteJavaScript(
			"document.addEventListener('transitionend', function(event){alert(event.target.textContent+' '+window.getComputedStyle(event.target, null).getPropertyValue('opacity'));}, false);",
			"", 0);*/


        // Set Tab's URL when page is loading
        _pMediator->SetURL(browser);

        // Set zoom level according to Tab's settings
        SetZoomLevel(browser, false);

        // Inject Javascript to hide scrollbar
        frame->ExecuteJavaScript(_js_remove_css_scrollbar, frame->GetURL(), 0);

    }
    else // Current frame is not the main frame
    {
        int64 mainFrameID = frame->GetParent()->GetIdentifier();
    }
}

void Handler::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode)
{
	// Set loading status of each frame in Tab to load finished in order to display loading icon and status
	_pMediator->SetLoadingStatus(browser, frame->GetIdentifier(), frame->IsMain(), false);


	if(frame->IsMain())
		LogDebug("Handler: End of loading frame id = ", frame->GetIdentifier(), " (main = ", frame->IsMain(), ").");

    // Inject Javascript to hide scrollbar
    frame->ExecuteJavaScript(_js_remove_css_scrollbar, frame->GetURL(), 0);

    if (frame->IsMain())
    {
        // Set zoom level according to Tab's settings
        SetZoomLevel(browser, false);
    }

}

void Handler::OnLoadingStateChange(CefRefPtr<CefBrowser> browser,
    bool isLoading,
    bool canGoBack,
    bool canGoForward)
{

    // TODO: There might be a more performant way to set those values not as regularly as with every callback
    _pMediator->SetCanGoBack(browser, canGoBack);
    _pMediator->SetCanGoForward(browser, canGoForward);

    // Finished loading whole page
    if (!isLoading)
    {
        // Set zoom level again at load end, in case it was written over again
        SetZoomLevel(browser, false);

        // Write page resolution to V8 variables, read them and update Tab
        UpdatePageResolution(browser);

    }
}

void Handler::CloseAllBrowsers(bool forceClose)
{
   LogDebug("Handler: Closing all browsers.");

    if (!CefCurrentlyOn(TID_UI))
    {
        // Execute on the UI thread.
        CefPostTask(TID_UI,
            base::Bind(&Handler::CloseAllBrowsers, this, forceClose));
        return;
    }

    if (_browserList.empty())
        return;

    BrowserList::const_iterator it = _browserList.begin();
    for (; it != _browserList.end(); ++it)
    {
        CloseBrowser(it->get());
    }

}

void Handler::CloseBrowser(CefRefPtr<CefBrowser> browser)
{
    int browserID = browser->GetIdentifier();

    browser->StopLoad();

    browser->GetHost()->CloseBrowser(false);

    LogDebug("Handler: Browser successfully closed (id = ", browserID, ").");
}

bool Handler::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
    CefProcessId source_process,
    CefRefPtr<CefProcessMessage> msg)
{
    const std::string& msgName = msg->GetName().ToString();

    if (msgName == "ReceiveDOMElements")
    {
        LogDebug("Handler: OLD APPROACH! DELETE! ... ReceiveDOMElements msg received -> redirect to CefMediator.");

        //_pMediator->ClearDOMNodes(browser);

        // Pipe IPC message to CefMediator in order to create DOM nodes
       /* _pMediator->ReceiveIPCMessageforDOM(browser, msg);*/
        return true;
    }
    if (msgName == "ReceiveFavIconBytes")
    {
        _pMediator->ReceiveIPCMessageforFavIcon(browser, msg);
    }
    if (msgName == "ReceivePageResolution")
    {
        _pMediator->ReceivePageResolution(browser, msg);
    }
    if (msgName == "ReceiveFixedElements")
    {
        _pMediator->ReceiveFixedElements(browser, msg);
    }
    if (msgName == "IPCLog")
    {
        IPCLogRenderer(browser, msg);
    }

	if (msgName == "OnContextCreated")
	{
		_pMediator->ClearDOMNodes(browser);
	}
	if (msgName == "SendDOMNodeData")
	{
		//_pMediator->HandleDOMNodeIPCMsg(browser, msg);
		LogDebug("Handler: Received old IPC msg 'SendDOMNodeData'!");
	}

	if (msgName.substr(0, 9) == "CreateDOM")
	{
		_pMediator->FillDOMNodeWithData(browser, msg);
	}

    return _msgRouter->OnProcessMessageReceived(browser, source_process, msg);
}

void Handler::ResizeBrowsers()
{
    CEF_REQUIRE_UI_THREAD();

    BrowserList::iterator bit = _browserList.begin();
    for (; bit != _browserList.end(); ++bit)
    {
        LogDebug("Handler: Browser (id = ", bit->get()->GetIdentifier(), " was resized.");

        bit->get()->GetHost()->WasResized();

        // Resize may cause change in page size
        UpdatePageResolution(bit->get());

		// TODO / NOTE: 
		// It might be better to perform that Rect Update not simultaneously on every browser.
		// Instead save it and execute it when you switch to the target tab

		bit->get()->GetMainFrame()->ExecuteJavaScript("UpdateDOMRects();", "", 0);

        // EXPERIMENTAL
        //GetFixedElements(bit->get());

    }
}

void Handler::LoadPage(CefRefPtr<CefBrowser> browser, std::string url)
{
    LogDebug("Handler: Loading page ", url);
    browser->GetMainFrame()->LoadURL(url);
}

void Handler::EmulateMouseCursor(CefRefPtr<CefBrowser> browser, double x, double y)
{
    CefMouseEvent event;
    event.x = x;
    event.y = y;
    browser->GetHost()->SendMouseMoveEvent(event, false);
}

void Handler::EmulateLeftMouseButtonClick(CefRefPtr<CefBrowser> browser, double x, double y)
{
    CefMouseEvent event;
    event.x = x;
    event.y = y;
    LogDebug("Handler: Emulating left mouse button click on position (", x, ", ", y, "), browserID = ", browser->GetIdentifier());
    browser->GetHost()->SendMouseClickEvent(event, MBT_LEFT, false, 1);	// press
    browser->GetHost()->SendMouseClickEvent(event, MBT_LEFT, true, 1);	// release

	// DEBUG
	//browser->GetMainFrame()->ExecuteJavaScript("consolePrint('BLABLABLA');StillObserving();", "", 0);
	//browser->GetMainFrame()->ExecuteJavaScript("ContextTest();", "", 0);
	//browser->GetMainFrame()->ExecuteJavaScript("StartObserving();", "", 0);
}

void Handler::EmulateMouseWheelScrolling(CefRefPtr<CefBrowser> browser, double deltaX, double deltaY)
{
    CefMouseEvent event;
    event.x = 0;	// TODO: Mouse position? Possible case: Scroll inside of a sub area inside the page
    event.y = 0;
    //DLOG(INFO) << "Emulating mouse wheel, browserID=" << browser->GetIdentifier();
    browser->GetHost()->SendMouseWheelEvent(event, deltaX, deltaY);
}

bool Handler::InputTextData(CefRefPtr<CefBrowser> browser, int64 frameID, int nodeID, std::string text, bool submit)
{
    //CEF_REQUIRE_UI_THREAD();

    CefRefPtr<CefFrame> frame = browser->GetFrame(frameID);
    if (frame->IsValid())
    {
        frame->ExecuteJavaScript(jsInputTextData(nodeID, text, submit), frame->GetURL(), 0);

        return true;
    }
    else
    {
        LogDebug("Handler: Tried to input text data, frame is not valid anymore.");
        return false;
    }
}

void Handler::Reload(CefRefPtr<CefBrowser> browser)
{
    LogDebug("Handler: Reloading browser (id = ", browser->GetIdentifier(), ") on page ", browser->GetMainFrame()->GetURL().ToString());
    browser->Reload();
}

void Handler::GoBack(CefRefPtr<CefBrowser> browser)
{
    browser->GoBack();
}

void Handler::GoForward(CefRefPtr<CefBrowser> browser)
{
    browser->GoForward();
}

void Handler::ResetMainFramesScrolling(CefRefPtr<CefBrowser> browser)
{
    const std::string resetScrolling = "document.body.scrollTop=0; document.body.scrollLeft=0;";
    browser->GetMainFrame()->ExecuteJavaScript(resetScrolling, browser->GetMainFrame()->GetURL(), 0);
}

void Handler::ReloadDOMNodes(CefRefPtr<CefBrowser> browser, std::string debug_info)
{
    //CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("GetDOMElements");
    //msg->GetArgumentList()->SetDouble(0, (double)browser->GetMainFrame()->GetIdentifier());	// Cast int64 frameID  to (64bit) double, int only has 32
    //browser->SendProcessMessage(PID_RENDERER, msg);
    //LogDebug("Handler: Sent \"GetDOMElements\" msg to renderer, if any listed node type exists ", debug_info);

	// TODO: Delete this method because of MutationObserver
}

void Handler::SetZoomLevel(CefRefPtr<CefBrowser> browser, bool definitelyChanged)
{

    if (double zoomLevel = _pMediator->GetZoomLevel(browser))
    {
        LogDebug("Handler: Setting zoom level = ", zoomLevel, " (browserID = ", browser->GetIdentifier(), ").");
		const std::string setZoomLevel = "document.body.style.zoom=" + std::to_string(zoomLevel) + ";this.blur(); UpdateDOMRects(); UpdateFixedElementRects();";
        browser->GetMainFrame()->ExecuteJavaScript(setZoomLevel, "", 0); 

        if (definitelyChanged)
        {
            // Reload DOM nodes because of changed coordinates due to zooming
            //_pMediator->ClearDOMNodes(browser);			// TODO: Rect update instead!

            ReloadDOMNodes(browser);

            UpdatePageResolution(browser);				// TODO: Does a JS event exist for this?

            // EXPERIMENTAL
            //GetFixedElements(browser);
        }
    }
}

void Handler::UpdatePageResolution(CefRefPtr<CefBrowser> browser)
{
    // Javascript code for receiving the current page width & height
    const std::string getPageResolution = "\
            window._pageWidth = document.documentElement.scrollWidth;\
            window._pageHeight = document.documentElement.scrollHeight;\
            ";

    browser->GetMainFrame()->ExecuteJavaScript(getPageResolution, browser->GetMainFrame()->GetURL(), 0);

    // Tell renderer process to get V8 values, where page sizes were written to
    CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("GetPageResolution");
    browser->SendProcessMessage(PID_RENDERER, msg);
}

void Handler::IPCLogRenderer(CefRefPtr<CefBrowser> browser, CefRefPtr<CefProcessMessage> msg)
{
    CefRefPtr<CefListValue> args = msg->GetArgumentList();
    const std::string text = args->GetString(1);
    const bool debugLog = args->GetBool(0);
    const int browserID = browser->GetIdentifier();

    if (debugLog)
    {
        LogDebug("Renderer: ", text, " (browserID = ", browserID, ")");
    }
}

void Handler::OnFaviconURLChange(CefRefPtr<CefBrowser> browser,
    const std::vector<CefString>& icon_urls)
{
    std::string highResURL;
    int currentRes = 0;
    std::string icoURL;

    for (int i = 0; i < (int)icon_urls.size(); i++)
    {
        std::string url = icon_urls[i].ToString();
        std::string format = "", res = "";

        // Read out file ending
        for (int j = url.size() - 1; j >= (int)url.size() - 7; j--)
        {
            if (url[j] == '.')
            {
                break;
            }
            format = url[j] + format;
        }

        if (format == "ico")
        {
            icoURL = url;
            continue;
        }

        // Try to read parts of resolution like "160x160" from URL's end
        for (int j = url.size() - format.size() - 7; j < (int)(url.size() - format.size()) - 1; j++)
        {
            res += url[j];
        }

        int height = 0;
        for (int j = res.size()-1; j >= 0; j--)
        {
            // char is a digit?
            if (digits.find(res[j]) != digits.end())
            {
                height += ((res[j]-'0') * pow(10, res.size()-1-j));
            }
            else
            {
                // break, when 'x' is reached
                break;
            }
        }

        if (height > currentRes)
        {
            highResURL = url;
            currentRes = height;
        }
    }

    const std::string iconURL = (currentRes > 0) ? highResURL : icoURL;

    // Trigger favIconImg.onload function by setting image src
    const std::string jscode = "favIconImg.src = '" + iconURL + "';";
    browser->GetMainFrame()->ExecuteJavaScript(jscode, "", 0); 

    // New image incoming, delete the last one
    _pMediator->ResetFavicon(browser);
}

void Handler::OnTitleChange(CefRefPtr<CefBrowser> browser,
	const CefString& title)
{
	_pMediator->OnTabTitleChange(browser, title.ToString());
}

bool Handler::OnBeforePopup(CefRefPtr<CefBrowser> browser,
	CefRefPtr<CefFrame> frame,
	const CefString& target_url,
	const CefString& target_frame_name,
	WindowOpenDisposition target_disposition,
	bool user_gesture,
	const CefPopupFeatures& popupFeatures,
	CefWindowInfo& windowInfo,
	CefRefPtr<CefClient>& client,
	CefBrowserSettings& settings,
	bool* no_javascript_access)
{
	LogInfo("Handler: Suppressed popup from opening a new window");

	_pMediator->OpenPopupTab(browser, target_url);

	return true;
}

void Handler::ScrollOverflowElement(CefRefPtr<CefBrowser> browser, int elemId, int x, int y)
{
	//DEBUG
	//LogDebug("Handler: Scrolling overflow element with id: ", elemId);

	std::string js_code = "var overflowObj = GetOverflowElement(" + std::to_string(elemId) + ");"\
		"if(overflowObj) overflowObj.scroll(" + std::to_string(x) + ", " + std::to_string(y) + ");";

	CefMouseEvent event;
	event.x = 0;	// TODO: Mouse position? Possible case: Scroll inside of a sub area inside the page
	event.y = 0;
	//DLOG(INFO) << "Emulating mouse wheel, browserID=" << browser->GetIdentifier();
	browser->GetHost()->SendMouseWheelEvent(event, 0, -10);

	// DEBUG
	if (elemId == 6 || elemId == 7)
	{
		// // Check if Rect update leads to Rect == 0 ... doesn't seem so.
		//js_code += "ConsolePrint('DEBUG| before: '+overflowObj.rects);" \
		//	"overflowObj.updateRects();" \
		//	"ConsolePrint('DEBUG| after: '+overflowObj.rects);";

		// // Check visbility when hidden again.. always true, opacity too
		//js_code += ("ConsolePrint(" + std::to_string(elemId) + "+\" visibility: \"+window.getComputedStyle(overflowObj.node, null).getPropertyValue('visibility') );");
		
		//js_code += ("ConsolePrint(" + std::to_string(elemId) + "+\" class: \"+overflowObj.node.className);");

		//js_code += ("ConsolePrint(\"C++ induced: \"+"+ std::to_string(elemId) + "+\" rect width: \"+overflowObj.node.getBoundingClientRect().width );");
	}

	browser->GetMainFrame()->ExecuteJavaScript(js_code, "",	0);

}
