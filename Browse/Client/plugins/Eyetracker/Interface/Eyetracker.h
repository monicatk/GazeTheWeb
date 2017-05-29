//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include <windows.h>
#define DLL_EXPORT __declspec(dllexport) // only on Windows

#include "plugins/Eyetracker/Interface/EyetrackerSampleData.h"
#include <vector>

// Export C interface (resolved overloading etc)
#ifdef __cplusplus
extern "C" {
#endif

	// Connect eyetracker, returns whether succesfull
	DLL_EXPORT bool Connect();

	// Check whether eyetracker is working (regardless of user presence)
	DLL_EXPORT bool IsTracking();

	// Disconnect eyetracker, returns whether succesfull
	DLL_EXPORT bool Disconnect();

	// Fetches gaze samples and clears buffer
	DLL_EXPORT void FetchSamples(std::vector<SampleData>& rSamples);

	// Perform calibration TODO: return something like an enum or so to provide user feedback
	DLL_EXPORT void Calibrate();

#ifdef __cplusplus
}
#endif