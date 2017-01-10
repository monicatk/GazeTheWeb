//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Mueller (muellerd@uni-koblenz.de)
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Implementation of CefApp for Main Process. Does implement mediator.

#ifndef CEF_MAINCEFAPP_H_
#define CEF_MAINCEFAPP_H_

#include "include/cef_app.h"
#include "src/CEF/Mediator.h"

class MainCefApp :	public CefApp,
					public CefBrowserProcessHandler,
					public Mediator
{
public:

    // Manipulate command line input
    virtual void OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr< CefCommandLine > command_line) OVERRIDE;

    // CefBrowserProcessHandler methods
    virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() OVERRIDE { return this; }
    virtual void OnContextInitialized() OVERRIDE;

private:

    // Include CEF'S default reference counting implementation
    IMPLEMENT_REFCOUNTING(MainCefApp);
};

#endif  // CEF_MAINCEFAPP_H_
