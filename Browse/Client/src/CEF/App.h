//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Müller (muellerd@uni-koblenz.de)
//============================================================================

#ifndef CEF_APP_H_
#define CEF_APP_H_

#include "include/cef_app.h"
#include "src/CEF/Extension/CefMediator.h"
#include "src/CEF/RenderProcess/RenderProcessHandler.h"

class App :	public CefApp,
            public CefBrowserProcessHandler,
            // expanding CefApp by our mediator interface
            public CefMediator
{
public:

    App();

    // Manipulate command line input
    virtual void OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr< CefCommandLine > command_line) OVERRIDE;

    // CefBrowserProcessHandler methods
    virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() OVERRIDE { return this; }
    virtual void OnContextInitialized() OVERRIDE;

    // Called for IPC message navigation
    CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() OVERRIDE {	return _renderProcessHandler; }

private:

    // Keep a reference to RenderProcessHandler
    CefRefPtr<RenderProcessHandler> _renderProcessHandler;

    // Include CEF'S default reference counting implementation
    IMPLEMENT_REFCOUNTING(App);
};

#endif  // CEF_APP_H_
