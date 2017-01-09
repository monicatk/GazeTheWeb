//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Mueller (muellerd@uni-koblenz.de)
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// CefApp implementation for Render Process.

#ifndef CEF_RENDERCEFAPP_H_
#define CEF_RENDERCEFAPP_H_

#include "include/cef_app.h"
#include "src/CEF/RenderProcess/RenderProcessHandler.h"

class RenderCefApp : public CefApp
{
public:

	// Constructor
	RenderCefApp();

	// Getter for render process handle
    CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() OVERRIDE {	return _renderProcessHandler; }

private:

    // Keep a reference to RenderProcessHandler
    CefRefPtr<RenderProcessHandler> _renderProcessHandler;

    // Include CEF'S default reference counting implementation
    IMPLEMENT_REFCOUNTING(RenderCefApp);
};

#endif // CEF_RENDERCEFAPP_H_
