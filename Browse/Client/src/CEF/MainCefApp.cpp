//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Mueller (muellerd@uni-koblenz.de)
//============================================================================

#include "MainCefApp.h"
#include "src/Setup.h"
#include "src/CEF/Handler.h"
#include "src/CEF/Renderer.h"
#include "src/CEF/DevToolsHandler.h"
#include "include/cef_browser.h"
#include "include/cef_command_line.h"
#include "include/wrapper/cef_helpers.h"
#include <string>

void MainCefApp::OnBeforeCommandLineProcessing(const CefString& process_type, CefRefPtr< CefCommandLine > command_line)
{
	// Enable WebGL part 2 (other is in CefMediator.cpp) (or not disable is better description)
	if (!setup::ENABLE_WEBGL)
	{
		// Setup for offscreen rendering
		command_line->AppendSwitch("disable-gpu");
		command_line->AppendSwitch("disable-gpu-compositing");
		command_line->AppendSwitch("enable-begin-frame-scheduling"); // breaks WebGL, but better for performance
	}

	command_line->AppendSwitch("enable-logging"); // get LOG(..) writes in console
	command_line->AppendSwitch("no-sandbox"); // for what necessary?

	// EXPERIMENTAL: slow loading?
	// see end of https://bitbucket.org/chromiumembedded/cef/wiki/GeneralUsage#markdown-header-proxy-resolution
	command_line->AppendSwitch("no-proxy-server");

	// EXPERIMENTAL
	// Javascript debugging?
	//command_line->AppendArgument("remote-debugging-port=666");
}

void MainCefApp::OnContextInitialized()
{
    CEF_REQUIRE_UI_THREAD();

	// TODO: handlers are members in Mediator but created here. Looks strange

    // Create Renderer
    CefRefPtr<Renderer> renderer(new Renderer(this));

    // Create Handler
	_handler = new Handler(this, renderer);

	// Create handler for dev tools
	_devToolsHandler = new DevToolsHandler();
}