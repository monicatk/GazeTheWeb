//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "EyeInput.h"
#include "src/Utils/Logger.h"
#include "src/Setup.h"
#include <cmath>
#include <functional>

EyeInput::EyeInput(bool useEmulation)
{

#ifdef _WIN32

	if (!useEmulation)
	{
		// Define procedure signature for connection
		typedef bool(__cdecl *CONNECT)();

		// Function to connect to eye tracker via plugin
		std::function<void(std::string)> ConnectEyeTracker = [&](std::string plugin)
		{
			std::string dllName = plugin + ".dll";
			_pluginHandle = LoadLibraryA(dllName.c_str());

			// Try to connect to eyetracker
			if (_pluginHandle != NULL)
			{
				LogInfo("EyeInput: Loaded " + plugin + ".");

				// Fetch procedure for connecting
				CONNECT procConnect = (CONNECT)GetProcAddress(_pluginHandle, "Connect");

				// Fetch procedure for fetching gaze data
				_procFetchGaze = (FETCH_GAZE)GetProcAddress(_pluginHandle, "FetchGaze");

				// Fetch procedure to check tracking
				_procIsTracking = (IS_TRACKING)GetProcAddress(_pluginHandle, "IsTracking");

				// Check whether procedures could be loaded
				if (procConnect != NULL && _procFetchGaze != NULL && _procIsTracking != NULL)
				{
					_connected = procConnect();
					if (_connected)
					{
						LogInfo("EyeInput: Connecting eyetracker successful.");
					}
					else
					{
						_procFetchGaze = NULL;
						_procIsTracking = NULL;
					}
				}
			}
			else
			{
				LogInfo("EyeInput: Failed to load " + plugin + ".");
			}
		};

		// Try to load SMI iViewX plugin
		if (!_connected)
		{
			ConnectEyeTracker("SMIiViewXPlugin");
		}

		// Try to load SMI myGaze plugin
		if (!_connected)
		{
			ConnectEyeTracker("SMImyGazePlugin");
		}

		// Try to load Tobii EyeX plugin
		if (!_connected)
		{
			ConnectEyeTracker("TobiiEyeXPlugin");
		}
	}

#endif

	if (!_connected)
	{
		LogInfo("EyeInput: No eyetracker connected. Input emulated by mouse.");
	}
}

EyeInput::~EyeInput()
{

#ifdef _WIN32

	if (_pluginHandle != NULL)
	{
		// Disconnect eyetracker if necessary
		if (_connected)
		{
			typedef bool(__cdecl *DISCONNECT)();
			DISCONNECT procDisconnect = (DISCONNECT)GetProcAddress(_pluginHandle, "Disconnect");

			// Disconnect eyetracker when procedure available
			if (procDisconnect != NULL)
			{
				bool result = procDisconnect();

				// Check whether disconnection has been successful
				if (result)
				{
					LogInfo("EyeInput: Disconnecting eyetracker successful.");
				}
				else
				{
					LogInfo("EyeInput: Disconnecting eyetracker failed.");
				}

				// Just set connection to false
				_connected = false;
			}
		}

		// Unload plugin
		FreeLibrary(_pluginHandle);
	}

#endif

}

bool EyeInput::Update(
	float tpf,
	double mouseX,
	double mouseY,
	double& rGazeX,
	double& rGazeY,
	int windowX,
	int windowY,
	int windowWidth,
	int windowHeight)
{
	// ### UPDATE GAZE INPUT ###

	// Update gaze by eyetracker
	double filteredGazeX = 0;
	double filteredGazeY = 0;

	// Bool whether eye tracker is tracking
	bool isTracking = false;

#ifdef _WIN32

	if (_connected && _procFetchGaze != NULL && _procIsTracking != NULL)
	{
		// Prepare vectors to fill
		std::vector<double> gazeXSamples, gazeYSamples;

		// Fetch k or less valid samples
		_procFetchGaze(EYETRACKER_AVERAGE_SAMPLE_COUNT, gazeXSamples, gazeYSamples);

		// Convert parameters to double (use same values for all samples,
		// although they could have been collected whil windows transformation has been different)
		double windowXDouble = (double)windowX;
		double windowYDouble = (double)windowY;
		double windowWidthDouble = (double)windowWidth;
		double windowHeightDouble = (double)windowHeight;

		// Average the given samples
		if (!gazeXSamples.empty())
		{
			double sum = 0;
			for (double x : gazeXSamples)
			{
				// Do some clamping according to window coordinates
				double clampedX = x - windowXDouble;
				clampedX = clampedX > 0.0 ? clampedX : 0.0;
				clampedX = clampedX < windowWidthDouble ? clampedX : windowWidthDouble;
				sum += clampedX;
			}
			filteredGazeX = sum / gazeXSamples.size();
		}
		if (!gazeYSamples.empty())
		{
			double sum = 0;
			for (double y : gazeYSamples)
			{
				// Do some clamping according to window coordinates
				double clampedY = y - windowYDouble;
				clampedY = clampedY > 0.0 ? clampedY : 0.0;
				clampedY = clampedY < windowHeightDouble ? clampedY : windowHeightDouble;
				sum += clampedY;
			}
			filteredGazeY = sum / gazeYSamples.size();
		}

		// Check, whether eye tracker is tracking
		isTracking = _procIsTracking();
	}

#endif

	// ### MOUSE INPUT ###

	// Mouse override of eyetracker
	if (_mouseOverride)
	{
		// Check whether override should stop
		if (mouseX == _mouseX && mouseY == _mouseY)
		{
			// Check whether override is over
			_mouseOverrideTime -= tpf;
			if (_mouseOverrideTime <= 0)
			{
				// Deactivate override
				_mouseOverride = false;
			}
		}
		else
		{
			// Since mouse cursor was moved, reset override (stop) time
			_mouseOverrideTime = EYEINPUT_MOUSE_OVERRIDE_STOP_DURATION;
		}
	}
	else
	{
		// Check whether override should start
		if (_mouseOverrideInitFrame)
		{
			// Already inside frame of initialization
			_mouseOverrideTime -= tpf;
			if (_mouseOverrideTime <= 0)
			{
				// Check whether mouse cursor movement was enough to trigger override
				float x = (float)(mouseX - _mouseOverrideX);
				float y = (float)(mouseY - _mouseOverrideY);
				float distance = std::sqrt(x*x + y*y);
				if (distance >= EYEINPUT_MOUSE_OVERRIDE_INIT_DISTANCE)
				{
					// Activate override
					_mouseOverride = true;
				}

				// Cleanup
				_mouseOverrideInitFrame = false;

				// Time is used while override to determine when to stop it
				_mouseOverrideTime = EYEINPUT_MOUSE_OVERRIDE_STOP_DURATION;
			}
		}
		else
		{
			// Check whether there was initial movement
			if (mouseX != _mouseX || mouseY != _mouseY)
			{
				// Start override initialization frame
				_mouseOverrideInitFrame = true;
				_mouseOverrideX = mouseX;
				_mouseOverrideY = mouseY;
				_mouseOverrideTime = EYEINPUT_MOUSE_OVERRIDE_INIT_FRAME_DURATION;
			}
		}
	}

	// ### GAZE EMULATION ###

	// Bool to indicate mouse usage for gaze coordinates
	bool gazeEmulated =
		!_connected // eyetracker not connected
		|| _mouseOverride // eyetracker overriden by mouse
		|| !isTracking; // eyetracker not available

	// Save mouse cursor coordinate in members for calculating mouse override
	_mouseX = mouseX;
	_mouseY = mouseY;

	// Use mouse when gaze is emulated
	if (gazeEmulated)
	{
		filteredGazeX = mouseX;
		filteredGazeY = mouseY;
	}

	// ### GAZE DISTORTION ###

	// Add optional noise and bias for testing purposes
	if (setup::EYEINPUT_DISTORT_GAZE)
	{
		filteredGazeX += setup::EYEINPUT_DISTORT_GAZE_BIAS_X;
		filteredGazeY += setup::EYEINPUT_DISTORT_GAZE_BIAS_Y;
	}

	// ### OUTPUT ###

	// Fill return values
	rGazeX = filteredGazeX;
	rGazeY = filteredGazeY;

	// Return whether gaze coordinates comes from eyetracker
	return !gazeEmulated;
}