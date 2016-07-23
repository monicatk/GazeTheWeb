#include "src/CEF/App.h"
#include "include/base/cef_logging.h"

// Forward declaration of common main
int CommonMain(const CefMainArgs& args, CefSettings settings, CefRefPtr<App> app, void* windows_sandbox_info, std::string userDirectory);

// Following taken out of CefSimple example of Chromium Embedded Framework!

// Entry point function for all processes.
int main(int argc, char* argv[])
{
  // Provide CEF with command-line arguments.
  CefMainArgs main_args(argc, argv);

  // SimpleApp implements application-level callbacks. It will create the first
  // browser instance in OnContextInitialized() after CEF has initialized.
  CefRefPtr<App> app(new App);

  // CEF applications have multiple sub-processes (render, plugin, GPU, etc)
  // that share the same executable. This function checks the command-line and,
  // if this is a sub-process, executes the appropriate logic.
  int exit_code = CefExecuteProcess(main_args, app.get(), NULL);
  if (exit_code >= 0)
  {
    // The sub-process has completed so return here.
    return exit_code;
  }

  // Specify CEF global settings here.
  CefSettings settings;

  // Fetch directory for saving bookmarks etc.
  std::string userDirectory = "~/.local/GazeTheWeb/Browse/";

  // Use common main now.
  return CommonMain(main_args, settings, app, NULL, userDirectory);
}
