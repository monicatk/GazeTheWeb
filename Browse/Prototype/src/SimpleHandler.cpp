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

#include <iostream>

namespace // instance only visible in this file
{

SimpleHandler* g_instance = NULL;

}

SimpleHandler::SimpleHandler(CefRefPtr<SimpleRenderer> renderer) : is_closing_(false)
{
      DCHECK(!g_instance);
      g_instance = this;
      m_renderer = renderer;
      newInputCoords = false;
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
    browser->GetHost()->SendFocusEvent(true);
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


// TODO: triggers breakpoint while shutting down?
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
    std::cout << "INFO: Page successfully loaded" << std::endl;
    // Send msg to renderer in order to fetch JS variables' values of input field coordinates
    CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("GetJSCoords");
    browser->SendProcessMessage(PID_RENDERER, msg);
    std::cout << "INFO: Send \"GetJSCoords\" msg to renderer" << std::endl;
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

bool SimpleHandler::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
    CefProcessId source_process,
    CefRefPtr<CefProcessMessage> msg)
{
    //std::cout << "INFO: SimpleHandler received a msg" << std::endl;
    // TODO: Receive input fields coordinates from renderer
    const std::string& msgName = msg->GetName().ToString();

    if (msgName == "JSCoords")
    {
        std::cout << "INFO: Received \"JSCoords\" msg from renderer." << std::endl;
        inputCoords.clear();
        for (unsigned int i = 0; i < msg->GetArgumentList()->GetSize()/4; i++)
        {
            if (i == 0) std::cout << "ID:\t(top,\tbottom,\tleft,\tright)" << std::endl;
            if (msg->GetArgumentList()->GetDouble(i * 4) >= 0)
            {
                inputCoords.push_back(
                    glm::vec4(
                    msg->GetArgumentList()->GetDouble(i * 4),		// top
                    msg->GetArgumentList()->GetDouble(i * 4 + 1),	// bottom
                    msg->GetArgumentList()->GetDouble(i * 4 + 2),	// left
                    msg->GetArgumentList()->GetDouble(i * 4 + 3)	// right
                        )
                    );
                std::cout << i <<":\t(" << inputCoords[i].x <<",\t"<< inputCoords[i].y <<",\t"<< inputCoords[i].z <<",\t"<< inputCoords[i].w << ")"<<std::endl;
            }
            else break;
        }
        // Test injecting text in input field without a keyboard
        //std::cout << "Emulating keyboard input..." << std::endl;
        //SetInputText(0, "input field #" + std::to_string(0) + "!");
        //SubmitInput(0);

        // Remember that new input coordinates were given
        newInputCoords = true;

        return true;
    }
	
	if (msgName == "InputSuccess")
	{
		std::cout << "INFO: InputSuccess msg received." << std::endl;
		return true;
	}


    if (msgName == "DEBUG")
    {
        std::cout << "DEBUG: Received DEBUG msg: \n>>> " + msg->GetArgumentList()->GetString(0).ToString();
        return true;
    }

    return false;
}

void SimpleHandler::SetInputText(unsigned int index, std::string txt)
{
    std::cout << "INFO: Received text input '" + txt + "' for input field #" << index << std::endl;

	// OLD
    //CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("KeyboardInput");
    //msg->GetArgumentList()->SetInt(0, index);
    //msg->GetArgumentList()->SetString(1, txt);
    //browser_list_.back()->SendProcessMessage(PID_RENDERER, msg);
    //std::cout << "INFO: Sending text input to renderer for injection via Javascript" << std::endl;

	std::stringstream ss;
	ss << "textInput[" + std::to_string(index) + "].value = '" + txt + "';";

	browser_list_.back()->GetFocusedFrame()->ExecuteJavaScript(ss.str(), browser_list_.back()->GetFocusedFrame()->GetURL(), 0);
	std::cout << "INFO: Successfully injected input text via JS." << std::endl;
}

void SimpleHandler::SubmitInput(unsigned int index)
{
    //CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("InputSubmit");
    //msg->GetArgumentList()->SetInt(0, index);
    //browser_list_.back()->SendProcessMessage(PID_RENDERER, msg);
}

bool SimpleHandler::CheckForNewInputFields()
{
    bool check = newInputCoords;
    newInputCoords = false;
    return check;
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
