//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "ProcessTypeGetter.h"

ProcessType ProcessTypeGetter::GetProcessType(CefRefPtr<CefCommandLine> commandLine)
{
	// This command line flag is not specified if is the main process aka browser process
	if (!commandLine->HasSwitch("type"))
	{
		return ProcessType::MAIN;
	}

	// Check for render process
	const std::string& processType = commandLine->GetSwitchValue("type");
	if (processType == "renderer")
	{
		return ProcessType::RENDER;
	}

	// Else, type is other (TODO: check Zygote process on linux...)
	return ProcessType::OTHER;
}