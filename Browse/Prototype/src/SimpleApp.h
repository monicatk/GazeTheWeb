//============================================================================
// Distributed under the MIT License.
// Author: Raphael Menges (https://github.com/raphaelmenges)
//============================================================================

#ifndef SIMPLE_APP_H
#define SIMPLE_APP_H

#include "include/cef_app.h"
#include "SimpleHandler.h"

#include "include/wrapper/cef_helpers.h"
#include "include/base/cef_logging.h"

/* Test for finding links in DOM-Tree (TODO: MOVE TO RENDER PROCESS?!) */
/*class LinkFinder : public CefDOMVisitor
{
public:
    LinkFinder() {mReady = false;}
    virtual void Visit(CefRefPtr<CefDOMDocument> document) OVERRIDE;
    bool isReady() const {return mReady;}
    std::string getTitle() const {return mTitle.ToString();}

private:
    bool mReady;
    CefString mTitle;
    IMPLEMENT_REFCOUNTING(LinkFinder);
};*/

// Simple app
class SimpleApp : public CefApp,
                  public CefBrowserProcessHandler {
 public:
  SimpleApp();

  // CefApp methods
  virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler()
      OVERRIDE { return this; }

  // CefBrowserProcessHandler methods
  virtual void OnContextInitialized() OVERRIDE;

    void SetStuff(unsigned int textureHandle, int* pWidth, int* pHeight)
    {
        mTextureHandle = textureHandle;
        mpWidth = pWidth;
        mpHeight = pHeight;
    }

    void SendMouseWheelEvent(const CefMouseEvent &event, int deltaX, int deltaY)
    {
        CEF_REQUIRE_UI_THREAD();
        mHandler->SendMouseWheelEvent(event, deltaX, deltaY);
    }

    void SendMouseClickEvent(const CefMouseEvent &event, bool up)
    {
        CEF_REQUIRE_UI_THREAD();
        mHandler->SendMouseClickEvent(event, up);
    }

    std::string GetLinks() const
    {
        // TODO (message from render process needed)
        return std::string();
    }

    // (TODO: MOVE TO RENDER PROCESS?!)
    /*virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefProcessId source_process, CefRefPtr<CefProcessMessage> message) OVERRIDE
    {
        CEF_REQUIRE_RENDERER_THREAD();
        browser->GetMainFrame()->LoadURL("http://www.google.de/");

        LOG(INFO) << message->GetName().ToString();

        if(message->GetName().compare("LinkFinder"))
        {
            browser->GetMainFrame()->LoadURL("http://www.google.de/"); //->VisitDOM(mLinkFinder);
        }
        return true;
    }*/

    // Load new url
    void loadNewURL(std::string url);

    // Go back
    void goBack();

    // Go forward
    void goForward();

	// Reload
	void reload();

    virtual void OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr< CefCommandLine > command_line);

 private:
  // Include the default reference counting implementation.
  IMPLEMENT_REFCOUNTING(SimpleApp);

  unsigned int mTextureHandle;
  int* mpWidth;
  int* mpHeight;
  CefRefPtr<SimpleHandler> mHandler;
  // CefRefPtr<LinkFinder> mLinkFinder; (TODO: MOVE TO RENDER PROCESS?!)
};

#endif  // SIMPLE_APP_H
