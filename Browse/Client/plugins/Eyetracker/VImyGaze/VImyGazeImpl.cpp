//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

// This is an implementation
#define DLL_IMPLEMENTATION

#include "plugins/Eyetracker/Interface/Eyetracker.h"
#include "plugins/Eyetracker/Common/EyetrackerData.h"
#include "myGazeAPI.h"
#include <algorithm>

// Global variables
static bool serverOwner = false;

int __stdcall SampleCallbackFunction(SampleStruct sampleData)
{
	// Get max from both eyes (assuming, that values at failure are smaller)
	double gazeX = std::max(sampleData.leftEye.gazeX, sampleData.rightEye.gazeX);
	double gazeY = std::max(sampleData.leftEye.gazeY, sampleData.rightEye.gazeY);

	// Push back to vector
	using namespace std::chrono;
	eyetracker_global::PushBackSample(
		SampleData(
			gazeX, // x
			gazeY, // y
			gazeX != 0 && gazeY != 0, // valid
			duration_cast<milliseconds>(
				system_clock::now().time_since_epoch() // timestamp
				)
		)
	);

	return 1;
}

bool Connect()
{
	// Initialize eyetracker
	SystemInfoStruct systemInfoData;
	int ret_connect = 0;

	// Connect to myGaze server
	ret_connect = iV_Connect(); // TODO BUG: never works, but it does for minimal sample code :(

	// If server not running, try to start it
	if (ret_connect != RET_SUCCESS)
	{
		// Start myGaze server
		iV_Start();

		// Retry to connect to myGaze server
		ret_connect = iV_Connect();

		// Remember to shut down server
		if (ret_connect == RET_SUCCESS)
		{
			serverOwner = true;
		}
	}

	// Set sample callback
	if (ret_connect == RET_SUCCESS)
	{
		/*
		iV_GetSystemInfo(&systemInfoData);
		LogInfo("iViewX ETSystem: ", systemInfoData.iV_ETDevice);
		LogInfo("iViewX iV_Version: ", systemInfoData.iV_MajorVersion, ".", systemInfoData.iV_MinorVersion, ".", systemInfoData.iV_Buildnumber);
		LogInfo("iViewX API_Version: ", systemInfoData.API_MajorVersion, ".", systemInfoData.API_MinorVersion, ".", systemInfoData.API_Buildnumber);
		LogInfo("iViewX SystemInfo Samplerate: ", systemInfoData.samplerate);
		*/

		// Define a callback function for receiving samples
		iV_SetSampleCallback(SampleCallbackFunction);
	}

	// Return success or failure
	return (ret_connect == RET_SUCCESS);
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

void Calibrate()
{
	// Start calibration (setup does not work of licensing reasons)
	int ret_calibrate = iV_Calibrate();
}