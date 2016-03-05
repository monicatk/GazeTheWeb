//============================================================================
// Distributed under the MIT License.
// Author: Raphael Menges (https://github.com/raphaelmenges)
//============================================================================

#include "SimpleApp.h"

#include <string>

#include "include/cef_browser.h"
#include "include/cef_command_line.h"
#include "include/wrapper/cef_helpers.h"

/* (TODO MOVE TO RENDER PROCESS?!)
void LinkFinder::Visit(CefRefPtr<CefDOMDocument> document)
{
    mReady = true;
    mTitle = document.get()->GetTitle();
} */

SimpleApp::SimpleApp()
{
   //  mLinkFinder = new LinkFinder();
}

void SimpleApp::OnContextInitialized() {
  CEF_REQUIRE_UI_THREAD();

  // Information used when creating the native window
  CefWindowInfo window_info;

#if defined(OS_WIN)
  // On Windows we need to specify certain flags that will be passed to
  // CreateWindowEx()
  window_info.SetAsPopup(NULL, "TODO");
#endif

  // Create renderer
  CefRefPtr<SimpleRenderer> renderer(new SimpleRenderer(mTextureHandle, mpWidth, mpHeight));

  // SimpleHandler implements browser-level callbacks
  mHandler = new SimpleHandler(renderer);

  // Specify CEF browser settings here.
  CefBrowserSettings browser_settings;

  std::string url;

  // Start page
  CefRefPtr<CefCommandLine> command_line =
      CefCommandLine::GetGlobalCommandLine();
  url = command_line->GetSwitchValue("url");
  if (url.empty())
    // Some offline page for testing without internet (living tough life in dorm)
    //url = "file://" + std::string(CONTENT_PATH) + "/websites/hello.html";
    url = "https://en.wikipedia.org/wiki/Main_Page";

  // Window handle set to zero (may cause visual errors)
  window_info.SetAsWindowless(0,false);

  // Create the first browser window
  CefBrowserHost::CreateBrowser(window_info, mHandler.get(), url,
                                browser_settings, NULL);
}

void SimpleApp::loadNewURL(std::string url)
{
    mHandler->loadNewURL(url);
}

void SimpleApp::goBack()
{
    mHandler->goBack();
}

void SimpleApp::goForward()
{
    mHandler->goForward();
}

void SimpleApp::reload()
{
	mHandler->reload();
}

void SimpleApp::OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr< CefCommandLine > command_line)
{
    // Setup for offscreen rendering
    command_line->AppendSwitch("disable-gpu");
    command_line->AppendSwitch("disable-gpu-compositing");
}
