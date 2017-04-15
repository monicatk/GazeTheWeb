//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include <windows.h>
#define DLL_EXPORT __declspec(dllexport) // only on Windows

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

	// Get k or less valid raw gaze entries. May fill vectors with nothing if nothing available
	DLL_EXPORT void FetchGaze(int maxSampleCount, std::vector<double>& rGazeX, std::vector<double>& rGazeY);

#ifdef __cplusplus
}
#endif