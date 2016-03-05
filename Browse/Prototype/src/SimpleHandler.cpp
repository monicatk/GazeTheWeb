//============================================================================
// Distributed under the MIT License.
// Author: Raphael Menges (https://github.com/raphaelmenges)
//============================================================================

#include "SimpleHandler.h"

#include <sstream>
#include <string>

#include "include/base/cef_bind.h"
#include "include/cef_app.h"
#include "include/wrapper/cef_closure_task.h"
#include "include/wrapper/cef_helpers.h"

namespace // instance only visible in this file
{

SimpleHandler* g_instance = NULL;

}

SimpleHandler::SimpleHandler(CefRefPtr<SimpleRenderer> renderer) : is_closing_(false)
{
      DCHECK(!g_instance);
      g_instance = this;
      m_renderer = renderer;
}

SimpleHandler::~SimpleHandler()
{
    // Kill the instance
    g_instance = NULL;
}

// static
SimpleHandler* SimpleHandler::GetInstance()
{
    return g_instance;
}

void SimpleHandler::OnTitleChange(CefRefPtr<CefBrowser> browser,
                                  const CefString& title)
{
    CEF_REQUIRE_UI_THREAD();
    // TODO
}

void SimpleHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
    // Add to the list of existing browsers
    browser_list_.push_back(browser);
}

bool SimpleHandler::DoClose(CefRefPtr<CefBrowser> browser) {
  CEF_REQUIRE_UI_THREAD();

  // Closing the main window requires special handling. See the DoClose()
  // documentation in the CEF header for a detailed destription of this
  // process.
  if (browser_list_.size() == 1) {
    // Set a flag to indicate that the window close should be allowed
    is_closing_ = true;
  }

  // Allow the close. For windowed browsers this will result in the OS close
  // event being sent
  return false;
}

void SimpleHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
  CEF_REQUIRE_UI_THREAD();

  // Remove from the list of existing browsers
  BrowserList::iterator bit = browser_list_.begin();
  for (; bit != browser_list_.end(); ++bit) {
    if ((*bit)->IsSame(browser)) {
      browser_list_.erase(bit);
      break;
    }
  }

  if (browser_list_.empty()) {
    // All browser windows have closed. Quit the application message loop
    CefQuitMessageLoop();
  }
}

void SimpleHandler::OnLoadError(CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefFrame> frame,
                                ErrorCode errorCode,
                                const CefString& errorText,
                                const CefString& failedUrl) {
  CEF_REQUIRE_UI_THREAD();

  // Don't display an error for downloaded files
  if (errorCode == ERR_ABORTED)
    return;

  // Display a load error message
  std::stringstream ss;
  ss << "<html><body bgcolor=\"white\">"
        "<h2>Failed to load URL " << std::string(failedUrl) <<
        " with error " << std::string(errorText) << " (" << errorCode <<
        ").</h2></body></html>";
  frame->LoadString(ss.str(), failedUrl);
}

void SimpleHandler::OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame)
{
    CEF_REQUIRE_UI_THREAD();

    // Inject Javascript do hide scrollbar (TODO: MOVE TO RENDER PROCESS?!)
    std::stringstream ss;
    ss << "var css = document.createElement(\"style\");"
        << "css.type = \"text/css\";"
        << "css.innerHTML = \"body::-webkit-scrollbar { width:0px !important; }\";"
        << "document.getElementsByTagName(\"head\")[0].appendChild(css);";

    frame->ExecuteJavaScript(ss.str(), "about:blank", 0);
}

void SimpleHandler::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode)
{
    CEF_REQUIRE_UI_THREAD();
    CefRefPtr<CefProcessMessage> message = CefProcessMessage::Create("LinkFinder");
    browser->SendProcessMessage(PID_RENDERER, message);
}

void SimpleHandler::CloseAllBrowsers(bool force_close) {
  if (!CefCurrentlyOn(TID_UI)) {
    // Execute on the UI thread.
    CefPostTask(TID_UI,
        base::Bind(&SimpleHandler::CloseAllBrowsers, this, force_close));
    return;
  }

  if (browser_list_.empty())
    return;

  BrowserList::const_iterator it = browser_list_.begin();
  for (; it != browser_list_.end(); ++it)
    (*it)->GetHost()->CloseBrowser(force_close);
}

void SimpleHandler::loadNewURL(std::string url)
{
    browser_list_.back()->GetMainFrame()->LoadURL(url);
}

void SimpleHandler::goBack()
{
    browser_list_.back()->GoBack();
}

void SimpleHandler::goForward()
{
    browser_list_.back()->GoForward();
}

void SimpleHandler::reload()
{
	browser_list_.back()->Reload();
}
