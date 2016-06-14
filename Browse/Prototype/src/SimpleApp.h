//============================================================================
// Distributed under the MIT License.
// Author: Daniel Müller (muellerd@uni-koblenz.de) & Raphael Menges (https://github.com/raphaelmenges)
//============================================================================

#ifndef SIMPLE_APP_H_
#define SIMPLE_APP_H_

#include "include/cef_app.h"
#include "SimpleHandler.h"

#include "include/wrapper/cef_helpers.h"
#include "include/base/cef_logging.h"

#include "SimpleRenderProcessHandler.h"
#include <iostream>

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

  CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() OVERRIDE
  {
	  return renderProcessHandler;
  }

  // CefBrowserProcessHandler methods
  virtual void OnContextInitialized() OVERRIDE;

    void SetStuff(unsigned int textureHandle, int* pWidth, int* pHeight, double* offsetX, double* offsetY)
    {
        mTextureHandle = textureHandle;
        mpWidth = pWidth;
        mpHeight = pHeight;
		scrollOffsetX = offsetX;
		scrollOffsetY = offsetY;
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

	// currently used in EntryPoint.h each while loop iteration --> pointer to shared memory instead of data copy?
	void setMouseCoordinates(double x, double y)
	{ 
		_x = x;
		_y = y;
		CefMouseEvent e;
		e.x = x;
		e.y = y;
		mHandler->MoveMouse(e);
	}

	// data in this location is updated when page is loaded, TODO: does eyeGUI need extra notifications after changes happened? -> YES
	std::vector<glm::vec4> const * GetInputCoordinateMemoryLocation() { return mHandler->GetInputCoordinateMemoryLocation(); }

	// |index| refers to input field coordinates' position in SimpleHandler's vector |inputCoords|
	void SetInputText(unsigned int index, std::string txt) { mHandler->SetInputText(index, txt); }

	void SubmitInputField(unsigned int index) { mHandler->SubmitInput(index); }

	bool CheckForNewInputFields() { return mHandler->CheckForNewInputFields(); }

 private:
	unsigned int mTextureHandle;
	int* mpWidth;
	int* mpHeight;
	double* scrollOffsetX;
	double* scrollOffsetY;

	CefRefPtr<SimpleHandler> mHandler;
	CefRefPtr<SimpleRenderProcessHandler> renderProcessHandler;
	// CefRefPtr<LinkFinder> mLinkFinder; (TODO: MOVE TO RENDER PROCESS?!)
 
	// current mouse position in web view coordinates
	double _x, _y;



	// Include the default reference counting implementation.
	IMPLEMENT_REFCOUNTING(SimpleApp); // read something about that this should be at the end..
};





#endif  // SIMPLE_APP_H_


