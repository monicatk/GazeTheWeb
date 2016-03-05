//============================================================================
// Distributed under the MIT License.
// Author: Raphael Menges (https://github.com/raphaelmenges)
//============================================================================

#ifndef SIMPLE_HANDLER_H_
#define SIMPLE_HANDLER_H_

#include "include/cef_client.h"
#include "SimpleRenderer.h"

#include "include/base/cef_logging.h"

#include <list>

// Responsible for single browser instance specific callbacks
class SimpleHandler : public CefClient,
                      public CefDisplayHandler,
                      public CefLifeSpanHandler,
                      public CefLoadHandler
{
public:
  SimpleHandler(CefRefPtr<SimpleRenderer> renderer);
  ~SimpleHandler();

  // Provide access to the single global instance of this object.
  static SimpleHandler* GetInstance();

  // CefClient methods
  virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() OVERRIDE {
    return this;
  }
  virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() OVERRIDE {
    return this;
  }
  virtual CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE {
    return this;
  }
  virtual CefRefPtr<CefRenderHandler> GetRenderHandler() OVERRIDE {
    return m_renderer;
  }

  // CefDisplayHandler methods
  virtual void OnTitleChange(CefRefPtr<CefBrowser> browser,
                             const CefString& title) OVERRIDE;

  // CefLifeSpanHandler methods
  virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
  virtual bool DoClose(CefRefPtr<CefBrowser> browser) OVERRIDE;
  virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) OVERRIDE;

  // CefLoadHandler methods
  virtual void OnLoadError(CefRefPtr<CefBrowser> browser,
                           CefRefPtr<CefFrame> frame,
                           ErrorCode errorCode,
                           const CefString& errorText,
                           const CefString& failedUrl) OVERRIDE;
  virtual void OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame) OVERRIDE;
  virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode) OVERRIDE;

  // Request that all existing browser windows close
  void CloseAllBrowsers(bool force_close);

  bool IsClosing() const { return is_closing_; }

  void SendMouseWheelEvent(const CefMouseEvent &event, int deltaX, int deltaY)
  {
    if(browser_list_.back().get() != NULL)
    {
        browser_list_.back().get()->GetHost()->SendMouseWheelEvent(event, deltaX, deltaY);
    }
  }

  void SendMouseClickEvent(const CefMouseEvent &event, bool up)
  {
    if(browser_list_.back().get() != NULL)
    {
        browser_list_.back()->GetHost()->SendMouseClickEvent(event, MBT_LEFT, up, 1);
    }
  }

  void loadNewURL(std::string url);

  void goBack();

  void goForward();

  void reload();

private:

  // List of existing browser windows. Only accessed on the CEF UI thread
  typedef std::list<CefRefPtr<CefBrowser> > BrowserList;
  BrowserList browser_list_;

  bool is_closing_;

  CefRefPtr<SimpleRenderer> m_renderer;

  // Include the default reference counting implementation
  IMPLEMENT_REFCOUNTING(SimpleHandler);
};

#endif  // SIMPLE_HANDLER_H_
