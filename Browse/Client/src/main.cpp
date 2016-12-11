//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#ifdef _WIN32
// Windows
#include "main_windows.h"
#elif __linux__
// Linux
#include "main_linux.h"
#endif

#include "src/Master.h"
#include "src/Utils/Logger.h"

// Execute function to have Master object on stack which might be faster than on heap
void Execute(CefRefPtr<App> app, std::string userDirectory)
{
    // Initialize master
    Master master(app.get(), userDirectory);

	// Give app poiner to master (only functions exposed through interface are accessible)
	app->SetMaster(&master);

    // Run master which communicates with CEF over mediator
    master.Run();

    // Destructor of master is called implicity
}

// Common main for linux and windows
int CommonMain(const CefMainArgs& args, CefSettings settings, CefRefPtr<App> app, void* windows_sandbox_info, std::string userDirectory)
{

#ifdef DEPLOYMENT

	// Disable logging of CEF
	settings.log_severity = LOGSEVERITY_DISABLE;

#else

	// Open Windows console for debugging purposes
#ifdef _WIN32
    AllocConsole();
    freopen("conin$", "r", stdin);
    freopen("conout$", "w", stdout);
    freopen("conout$", "w", stderr);
#endif

#endif

	// TODO: TESTING PLUGIN LOADING
	typedef int(__cdecl *MYPROC)();
	HINSTANCE hinstLib;
	MYPROC ProcAdd;
	BOOL fFreeResult, fRunTimeLinkSuccess = FALSE;

	// Get handle to library
	hinstLib = LoadLibrary(TEXT("mylibrary.dll"));

	// Check whether library has ben found
	if (hinstLib != NULL)
	{
		LogInfo("LibraryLoaded");
		ProcAdd = (MYPROC)GetProcAddress(hinstLib, "HelloWorld");

		// If the function address is valid, call the function.
		if (NULL != ProcAdd)
		{
			fRunTimeLinkSuccess = TRUE;
			int result = (ProcAdd)();
			LogInfo("Result is: ", result);
		}

		// Free the DLL module.
		fFreeResult = FreeLibrary(hinstLib);
	}

	// Set path for CEF data, both cache and user data
	CefString(&settings.cache_path).FromASCII(std::string(userDirectory + "cache").c_str());
	CefString(&settings.user_data_path).FromASCII(std::string(userDirectory + "user_data").c_str());

	// Set output path of log file
	LogPath = userDirectory;

	// Say hello
	LogInfo("####################################################");
	LogInfo("Welcome to GazeTheWeb - Browse!");
	LogInfo("Personal files are saved in: ", userDirectory);

	// Turn on offscreen rendering.
	settings.windowless_rendering_enabled = true;
	settings.remote_debugging_port = 8088;

    // Initialize CEF
    LogInfo("Initializing CEF...");
    CefInitialize(args, settings, app.get(), windows_sandbox_info);
    LogInfo("..done.");

    // Execute our code
    Execute(app, userDirectory);

    // Shutdown CEF
    LogInfo("Shutdown CEF...");
    CefShutdown();
    LogInfo("..done.");

    // Return zero
    LogInfo("Successful termination of program.");
    LogInfo("####################################################");
    return 0;
}
