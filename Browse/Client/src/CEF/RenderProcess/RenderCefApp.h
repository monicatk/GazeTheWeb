//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Müller (muellerd@uni-koblenz.de)
//============================================================================

#ifndef CEF_RENDERCEFAPP_H_
#define CEF_RENDERCEFAPP_H_

#include "include/cef_app.h"
#include "src/CEF/RenderProcess/RenderProcessHandler.h"

class RenderCefApp :	public CefApp
{
public:

	RenderCefApp();
    CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() OVERRIDE {	return _renderProcessHandler; }

private:

    // Keep a reference to RenderProcessHandler
    CefRefPtr<RenderProcessHandler> _renderProcessHandler;

    // Include CEF'S default reference counting implementation
    IMPLEMENT_REFCOUNTING(RenderCefApp);
};

#endif  // CEF_RENDERCEFAPP_H_
