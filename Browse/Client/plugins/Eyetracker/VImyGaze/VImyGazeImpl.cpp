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

bool Connect()
{
	// Initialize eyetracker
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
		// Get system info
		SystemInfoStruct systemInfoData;
		iV_GetSystemInfo(&systemInfoData); // not used right now

		// Setup LabStreamingLayer
		lsl::stream_info streamInfo(
			"myGazeLSL",
			"Gaze",
			2, // must match with number of samples in SampleData structure
			lsl::IRREGULAR_RATE, // otherwise will generate samples even if transmission paused (and somehow even gets the "real" samples, no idea how)
			lsl::cf_double64, // must match with type of samples in SampleData structure
			"source_id");
		streamInfo.desc().append_child_value("manufacturer", "Visual Interaction GmbH");
		lsl::xml_element channels = streamInfo.desc().append_child("channels");
		channels.append_child("channel")
			.append_child_value("label", "gazeX")
			.append_child_value("unit", "screenPixels");
		channels.append_child("channel")
			.append_child_value("label", "gazeY")
			.append_child_value("unit", "screenPixels");
		eyetracker_global::SetupLabStream(streamInfo);

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

bool Calibrate()
{
	// Start calibration (setup does not work of licensing reasons)
	return iV_Calibrate() == RET_SUCCESS;
}

void ContinueLabStream()
{
	eyetracker_global::ContinueLabStream();
}

void PauseLabStream()
{
	eyetracker_global::PauseLabStream();
}