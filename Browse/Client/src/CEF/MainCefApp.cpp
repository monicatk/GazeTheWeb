//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Müller (muellerd@uni-koblenz.de)
//============================================================================

#include "MainCefApp.h"
#include "src/Setup.h"
#include "src/CEF/Handler.h"
#include "src/CEF/Renderer.h"
#include "include/cef_browser.h"
#include "include/cef_command_line.h"
#include "include/wrapper/cef_helpers.h"
#include <string>

MainCefApp::MainCefApp() : Mediator()
{
	// Nothing to do here
}

void MainCefApp::OnContextInitialized()
{
    CEF_REQUIRE_UI_THREAD();

    // Create Renderer
    CefRefPtr<Renderer> renderer(new Renderer(this));

    // Create Handler with knowledge of CefMediator and Renderer
	_handler = new Handler(this, renderer);
}

void MainCefApp::OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr< CefCommandLine > command_line)
{
    // Enable WebGL part 2 (other is in CefMediator.cpp) (or not disable is better description)
    if(!setup::ENABLE_WEBGL)
    {
        // Setup for offscreen rendering
        command_line->AppendSwitch("disable-gpu");
        command_line->AppendSwitch("disable-gpu-compositing");
        command_line->AppendSwitch("enable-begin-frame-scheduling"); // breaks WebGL, but better for performance
    }

    command_line->AppendSwitch("enable-logging");	// get LOG(..) writes in console
    command_line->AppendSwitch("no-sandbox");		// enable logging in renderer process

    // EXPERIMENTAL: slow loading?
    // see end of https://bitbucket.org/chromiumembedded/cef/wiki/GeneralUsage#markdown-header-proxy-resolution
    command_line->AppendSwitch("no-proxy-server");

	// EXPERIMENTAL
	// Javascript debugging?
	//command_line->AppendArgument("remote-debugging-port=666");
}