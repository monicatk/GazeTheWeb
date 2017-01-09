//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Mueller (muellerd@uni-koblenz.de)
//============================================================================

#include "src/CEF/DevToolsHandler.h"
#include "include/base/cef_bind.h"
#include "include/cef_app.h"
#include "include/wrapper/cef_closure_task.h"
#include "include/wrapper/cef_helpers.h"
#include <sstream>
#include <string>

void DevToolsHandler::OnTitleChange(
	CefRefPtr<CefBrowser> browser,
	const CefString& title)
{
	// Just handling an "unresolved external symbol" error
}

void DevToolsHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{  
	CEF_REQUIRE_UI_THREAD();
	browser_list_.push_back(browser);
}

bool DevToolsHandler::DoClose(CefRefPtr<CefBrowser> browser)
{
	CEF_REQUIRE_UI_THREAD();
	if (browser_list_.size() == 1)
	{
		// All browsers closed, one is allowed to close the window
		is_closing_ = true;
	}
	return false;
}

void DevToolsHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser)
{
	CEF_REQUIRE_UI_THREAD();

	// Remove from the list of existing browsers
	BrowserList::iterator bit = browser_list_.begin();
	for (; bit != browser_list_.end(); bit++)
	{
		if ((*bit)->IsSame(browser))
		{
			browser_list_.erase(bit);
			break;
		}
	}
}

void DevToolsHandler::OnLoadError(
	CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    ErrorCode errorCode,
    const CefString& errorText,
    const CefString& failedUrl)
{
	CEF_REQUIRE_UI_THREAD();

	// Don't display an error for downloaded files
	if (errorCode == ERR_ABORTED)
	{
		return;
	}

	// Display a load error message
	std::stringstream ss;
	ss << "<html><body bgcolor=\"white\">"
		"<h2>Failed to load URL " << std::string(failedUrl) <<
		" with error " << std::string(errorText) << " (" << errorCode <<
		").</h2></body></html>";
	frame->LoadString(ss.str(), failedUrl);
}

void DevToolsHandler::CloseAllBrowsers(bool force_close)
{
	CEF_REQUIRE_UI_THREAD();

	if (browser_list_.empty())
	return;

	BrowserList::const_iterator bit = browser_list_.begin();
	for (; bit != browser_list_.end(); bit++)
	{
		(*bit)->GetHost()->CloseBrowser(force_close);
	}
}