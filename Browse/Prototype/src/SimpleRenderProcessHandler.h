//============================================================================
// Distributed under the MIT License.
// Author: Daniel Müller (muellerd@uni-koblenz.de)
//============================================================================

#ifndef SIMPLE_RENDERPROCESSHANDLER_H_
#define SIMPLE_RENDERPROCESSHANDLER_H_

#include "include/cef_render_process_handler.h"
//#include "include/wrapper/cef_helpers.h"

class SimpleRenderProcessHandler : public CefRenderProcessHandler {
public:
	SimpleRenderProcessHandler() {}

	~SimpleRenderProcessHandler(){}

	virtual bool OnProcessMessageReceived(
		CefRefPtr<CefBrowser> browser,
		CefProcessId sourceProcess,
		CefRefPtr<CefProcessMessage> msg) OVERRIDE;

	virtual void OnFocusedNodeChanged(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefDOMNode> node) OVERRIDE;


	void OnContextCreated(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefV8Context> context) OVERRIDE;

	//virtual void OnRenderThreadCreated(CefRefPtr<CefListValue> extra_info) OVERRIDE;

private:
	const unsigned int FETCHED_JS_TEXT_INPUTS = 10;
	IMPLEMENT_REFCOUNTING(SimpleRenderProcessHandler);
};



#endif // SIMPLE_RENDERPROCESSHANDLER_H_