//============================================================================
// Distributed under the MIT License.
// Author: Raphael Menges (https://github.com/raphaelmenges)
//============================================================================

#include <windows.h>

#include "src/SimpleApp.h"
#include "src/EntryPoint.h"
#include "include/cef_sandbox_win.h"

#if defined(CEF_USE_SANDBOX)
#pragma comment(lib, "cef_sandbox.lib")
#endif

// Entry point function for all processes.
int APIENTRY wWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
	{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	//CefEnableHighDPISupport();

	void* sandbox_info = NULL;

#if defined(CEF_USE_SANDBOX)
	// Manage the life span of the sandbox information object. This is necessary
	// for sandbox support on Windows. See cef_sandbox_win.h for complete details.
	CefScopedSandboxInfo scoped_sandbox;
	sandbox_info = scoped_sandbox.sandbox_info();
#endif

	// Provide CEF with command-line arguments
	CefMainArgs main_args(hInstance);

	// Application level callbacks are implemented by SimpleApp
	CefRefPtr<SimpleApp> app(new SimpleApp);

	// Decide subprocess or not
	int exit_code = CefExecuteProcess(main_args, app.get(), sandbox_info);
	if (exit_code >= 0) {

		// The sub-process has completed so return here.
		return exit_code;
	}

	// Settings
	CefSettings settings;
	settings.windowless_rendering_enabled = true;

#if !defined(CEF_USE_SANDBOX)
	settings.no_sandbox = true;
#endif

	// Entry point
	entry(main_args, settings, app.get(), sandbox_info);

	// Shutdown
	CefShutdown();

	return 0;
}
