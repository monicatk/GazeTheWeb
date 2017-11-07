//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

// This is an implementation
#define DLL_IMPLEMENTATION

#include "plugins/Eyetracker/Interface/Eyetracker.h"
#include "plugins/Eyetracker/Common/EyetrackerData.h"
#include "iViewXAPI.h"
#include <algorithm>

// Global variables
static bool serverOwner = false;

int __stdcall SampleCallbackFunction(SampleStruct sampleData)
{
	// Get max from both eyes (assuming, that values at failure are smaller)
	double gazeX = std::max(sampleData.leftEye.gazeX, sampleData.rightEye.gazeX);
	double gazeY = std::max(sampleData.leftEye.gazeY, sampleData.rightEye.gazeY);

	// Push back to vector
	if (gazeX != 0 && gazeY != 0) // push only valid samples
	{
		using namespace std::chrono;
		eyetracker_global::PushBackSample(
			SampleData(
				gazeX, // x
				gazeY, // y
				SampleDataCoordinateSystem::SCREEN_PIXELS,
				duration_cast<milliseconds>(
					system_clock::now().time_since_epoch() // timestamp
					)
			)
		);
	}

	return 1;
}

EyetrackerInfo Connect(EyetrackerGeometry geometry)
{
	// TODO: use provided geometry similar to myGaze plugin

	// Variables
	EyetrackerInfo info;
	int ret_connect = 0;

	// Connect to iViewX server
	ret_connect = iV_ConnectLocal();

	// If server not running, try to start it
	if (ret_connect != RET_SUCCESS)
	{
		// Start iViewX server
		iV_Start(iViewNG);

		// Retry to connect to iViewX server
		ret_connect = iV_ConnectLocal();

		// Remember to shut down server
		if (ret_connect == RET_SUCCESS)
		{
			serverOwner = true;
		}
	}

	// Set sample callback
	if (ret_connect == RET_SUCCESS)
	{
		// Connection successful
		info.connected = true;

		// Get system info
		SystemInfoStruct systemInfoData;
		iV_GetSystemInfo(&systemInfoData);
		info.samplerate = systemInfoData.samplerate;

		// Define a callback function for receiving samples
		iV_SetSampleCallback(SampleCallbackFunction);
	}

	// Return info structure
	return info;
}

bool IsTracking()
{
	return true;
}

bool Disconnect()
{
	// Disable callbacks
	iV_SetSampleCallback(NULL);

	// Disconnect
	if (serverOwner) // also shutdown server
	{
		return iV_Quit() == RET_SUCCESS;
	}
	else
	{
		return iV_Disconnect() == RET_SUCCESS;
	}
}

void FetchSamples(SampleQueue& rspSamples)
{
	eyetracker_global::FetchSamples(rspSamples);
}

CalibrationResult Calibrate(CalibrationInfo& rspInfo)
{
	// Start calibration
	return iV_Calibrate() == RET_SUCCESS ? CALIBRATION_OK : CALIBRATION_FAILED;
}

void ContinueLabStream()
{
	eyetracker_global::ContinueLabStream();
}

void PauseLabStream()
{
	eyetracker_global::PauseLabStream();
}