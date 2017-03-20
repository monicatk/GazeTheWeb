//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#ifndef CEF_PROCESSTYPEGETTER_H_
#define CEF_PROCESSTYPEGETTER_H_

#include "include/cef_command_line.h"

enum class ProcessType
{
	MAIN, // aka CEF browser process
	RENDER, // aka CEF renderer process
	OTHER
};

namespace ProcessTypeGetter
{
	// Implementation of process type getting
	ProcessType GetProcessType(CefRefPtr<CefCommandLine> command_line);
}

#endif // CEF_PROCESSTYPEGETTER_H_
