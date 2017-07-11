//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

// This is an implementation
#define DLL_IMPLEMENTATION

#include "plugins/Eyetracker/Interface/Eyetracker.h"
#include "plugins/Eyetracker/Common/EyetrackerData.h"
#include <algorithm>

bool Connect()
{
	// Return success or failure
	return false;
}

bool IsTracking()
{
	return true;
}

bool Disconnect()
{
	return true;
}

void FetchSamples(SampleQueue& rspSamples)
{
	eyetracker_global::FetchSamples(rspSamples);
}

bool Calibrate()
{
	// Not yet implemented
	return false;
}