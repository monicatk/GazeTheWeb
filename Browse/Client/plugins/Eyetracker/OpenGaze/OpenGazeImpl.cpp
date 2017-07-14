//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

// This is an implementation
#define DLL_IMPLEMENTATION

// Defines minimal Windows version for GPClient
#include <WinSDKVer.h>
#define _WIN32_WINNT_WIN7 // Windows 7 or higher

#include "plugins/Eyetracker/Interface/Eyetracker.h"
#include "plugins/Eyetracker/Common/EyetrackerData.h"
#include "plugins/Eyetracker/OpenGaze/include/GPClient.h"
#include <algorithm>

// TODO remove
#include <iostream>

// GazePoint standard client
GPClient client;
int screenX, screenY, screenWidth, screenHeight = 0;

bool Connect()
{
	// Try to connect to the client
	client.client_connect();

	std::cout << "Connecting" << std::endl;

	// Wait some time
	Sleep(500);

	// Check connection
	bool connected = client.is_connected();
	if (connected)
	{
		// Retrieve screen size of screen where eye tracker is attached
		client.send_cmd("<GET ID=\"SCREEN_SIZE\" />");
	}

	// TODO: what happens when connection succeeds after sleep time?

	std::cout << "Connection Status: " << connected << std::endl;

	// Return success or failure
	return connected;
}

bool IsTracking()
{
	return true;
}

bool Disconnect()
{
	client.client_disconnect();
	return true;
}

void FetchSamples(SampleQueue& rspSamples)
{
	// No callback is used by eye tracker, but samples are fetched here
	float x, y;
	int valid;
	std::deque<std::string> messages;

	// Retrieve and process as many messages as available
	client.get_rx(messages); // latest message
	for (auto& rMessage : messages) // should start at oldest (FIFO)
	{
		// API response to screen size query, extract data then start the data sending
		if (rMessage.find("<ACK ID=\"SCREEN_SIZE\"") != string::npos)
		{
			// Extract data
			sscanf_s(rMessage.c_str(), "<ACK ID=\"SCREEN_SIZE\" X=\"%i\" Y=\"%i\" WIDTH=\"%i\" HEIGHT=\"%i\" />", &screenX, &screenY, &screenWidth, &screenHeight);

			// Tell server what to stream
			client.send_cmd("<SET ID=\"ENABLE_SEND_POG_BEST\" STATE=\"1\" />");
		}
		// API data record, extract useful info
		else if (rMessage.find("<REC ") != string::npos)
		{
			// Extract data
			sscanf_s(rMessage.c_str(), "<REC BPOGX=\"%f\" BPOGY=\"%f\" BPOGV=\"%d\"/>",
				&x, &y, &valid);

			// Only add valid samples
			if (valid > 0)
			{
				// Scale to pixels and offset to handle multi-monitor possibility
				x = x * screenWidth + screenX;
				y = y * screenHeight + screenY;

				// TODO: use timestamp from API, not local one. but in which relation is that timestamp?
				using namespace std::chrono;
				eyetracker_global::PushBackSample(
					SampleData(
						x, // x
						y, // y
						SampleDataCoordinateSystem::SCREEN_PIXELS,
						duration_cast<milliseconds>(
							system_clock::now().time_since_epoch() // timestamp
							)
					)
				);
			}
		}
	}

	// Finally, fetch collected samples
	eyetracker_global::FetchSamples(rspSamples);
}

bool Calibrate()
{
	// Not yet implemented
	return false;
}