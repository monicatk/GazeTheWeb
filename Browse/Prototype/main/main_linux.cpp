//============================================================================
// Distributed under the MIT License.
// Author: Raphael Menges (https://github.com/raphaelmenges)
//============================================================================

#include "src/SimpleApp.h"
#include "src/EntryPoint.h"

// Entry point function for all processes
int main(int argc, char* argv[])
{
    // Provide CEF with command-line arguments
    CefMainArgs main_args(argc, argv);

    // Application level callbacks are implemented by SimpleApp
    CefRefPtr<SimpleApp> app(new SimpleApp);

    // Decide subprocess or not
    int exit_code = CefExecuteProcess(main_args, app.get(), NULL);
    if (exit_code >= 0) {

        // The sub-process has completed so return here
        return exit_code;
    }

    // Settings
    CefSettings settings;

    // Entry point
    entry(main_args, settings, app.get(), NULL);

    // Shutdown
    CefShutdown();

   return 0;
}
