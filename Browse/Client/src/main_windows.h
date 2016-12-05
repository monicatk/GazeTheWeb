#include <windows.h>
#include "src/CEF/App.h"
#include "include/cef_sandbox_win.h"

#if defined(CEF_USE_SANDBOX)
#pragma comment(lib, "cef_sandbox.lib")
#endif

// Forward declaration of common main
int CommonMain(const CefMainArgs& args, CefSettings settings, CefRefPtr<App> app, void* windows_sandbox_info, std::string userDirectory);

// Following taken partly out of CefSimple example of Chromium Embedded Framework!

// Entry point function for all processes.
int APIENTRY wWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow) {
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// Enable High-DPI support on Windows 7 or newer.
	// CefEnableHighDPISupport();

	// Sandbox information.
	void* sandbox_info = NULL;

#if defined(CEF_USE_SANDBOX)
	// Manage the life span of the sandbox information object. This is necessary
	// for sandbox support on Windows. See cef_sandbox_win.h for complete details.
	CefScopedSandboxInfo scoped_sandbox;
	sandbox_info = scoped_sandbox.sandbox_info();
#endif

	// Provide CEF with command-line arguments.
	CefMainArgs main_args(hInstance);

	// App implements application-level callbacks. It will create the first
	// browser instance in OnContextInitialized() after CEF has initialized.
	CefRefPtr<App> app(new App);

	// CEF applications have multiple sub-processes (render, plugin, GPU, etc)
	// that share the same executable. This function checks the command-line and,
	// if this is a sub-process, executes the appropriate logic.
	int exit_code = CefExecuteProcess(main_args, app.get(), sandbox_info);
	if (exit_code >= 0) {
		// The sub-process has completed so return here.
		return exit_code;
	}

	// Specify CEF global settings here.
	CefSettings settings;

#if !defined(CEF_USE_SANDBOX)
	settings.no_sandbox = true;
#endif

	// Fetch directory for saving bookmarks etc.
	char *pValue;
	size_t len;
	// errno_t err = _dupenv_s(&pValue, &len, "APPDATA"); // not used err produces compiler warning
	_dupenv_s(&pValue, &len, "APPDATA");
	std::string userDirectory(pValue, len - 1);

    // Create project folder.
	userDirectory.append("\\GazeTheWeb");
	if (CreateDirectoryA(userDirectory.c_str(), NULL) ||
		ERROR_ALREADY_EXISTS == GetLastError())
	{
        // Everything ok.
	}
	else
	{
        // Folder could not be created.
	}

    // Create application folder.
	userDirectory.append("\\Browse");
	if (CreateDirectoryA(userDirectory.c_str(), NULL) ||
		ERROR_ALREADY_EXISTS == GetLastError())
	{
        // Everything ok.
	}
	else
	{
        // Folder could not be created.
	}

    // Append another slash for easier usage of that path.
	userDirectory.append("\\");

	// Use common main now.
	return CommonMain(main_args, settings, app, sandbox_info, userDirectory);
}
