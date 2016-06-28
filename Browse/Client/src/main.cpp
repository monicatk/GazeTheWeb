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
void Execute(CefRefPtr<App> app)
{
    // Initialize master
    Master master(app.get());

    // Run master which communicates with CEF over mediator
    master.Run();

    // Destructor of master is called implicity
}

// Common main for linux and windows
int CommonMain(const CefMainArgs& args, CefSettings settings, CefRefPtr<App> app, void* windows_sandbox_info)
{
    LogInfo("####################################################");
    LogInfo("Welcome to GazeTheWeb - Browse!");

    // Turn on offscreen rendering.
    settings.windowless_rendering_enabled = true;

#ifndef DEPLOYMENT
#ifdef _WIN32
    // Open Windows console for debugging purposes
    AllocConsole();
    freopen("conin$", "r", stdin);
    freopen("conout$", "w", stdout);
    freopen("conout$", "w", stderr);
#endif
#endif

    // Initialize CEF
    LogInfo("Initializing CEF...");
    CefInitialize(args, settings, app.get(), windows_sandbox_info);
    LogInfo("..done.");

    // Execute our code
    Execute(app);

    // Shutdown CEF
    LogInfo("Shutdown CEF...");
    CefShutdown();
    LogInfo("..done.");

    // Return zero
    LogInfo("Successful termination of program.");
    LogInfo("####################################################");
    return 0;
}
