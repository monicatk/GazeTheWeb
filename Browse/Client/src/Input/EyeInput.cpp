//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "EyeInput.h"
#include "src/Utils/Logger.h"
#include "src/Setup.h"
#include <cmath>
#include <functional>

EyeInput::EyeInput(StatusCallback callback)
{
	// Store callback
	_statusCallback = callback;

	// Create thread for connection to eye tracker
	_upConnectionThread = std::unique_ptr<std::thread>(new std::thread([this]()
	{
#ifdef _WIN32

		// Trying to connect
		_statusCallback(EyeInput::Status::TRYING_TO_CONNECT);

		// Define procedure signature for connection
		typedef bool(__cdecl *CONNECT)();

		// Function to connect to eye tracker via plugin
		std::function<void(std::string)> ConnectEyeTracker = [&](std::string plugin)
		{
			std::string dllName = plugin + ".dll";
			_pluginHandle = LoadLibraryA(dllName.c_str());

			// Try to connect to eye tracker
			if (_pluginHandle != NULL)
			{
				LogInfo("EyeInput: Loaded " + plugin + ".");

				// Fetch procedure for connecting
				CONNECT procConnect = (CONNECT)GetProcAddress(_pluginHandle, "Connect");

				// Fetch procedure for fetching gaze data
				_procFetchGaze = (FETCH_GAZE)GetProcAddress(_pluginHandle, "FetchGaze");

				// Fetch procedure to check tracking
				_procIsTracking = (IS_TRACKING)GetProcAddress(_pluginHandle, "IsTracking");

				// Fetch procedure to calibrate
				_procCalibrate = (CALIBRATE)GetProcAddress(_pluginHandle, "Calibrate");

				// Check whether procedures could be loaded
				if (procConnect != NULL && _procFetchGaze != NULL && _procIsTracking != NULL && _procCalibrate != NULL)
				{
					_connected = procConnect();
					if (_connected)
					{
						LogInfo("EyeInput: Connecting eye tracker successful.");
					}
					else
					{
						// Reset handles when connection to eye tracker failed
						_procFetchGaze = NULL;
						_procIsTracking = NULL;
						_procCalibrate = NULL;
					}
				}
			}
			else
			{
				LogInfo("EyeInput: Failed to load " + plugin + ".");
			}
		};

		// Try to load SMI iViewX plugin
		if (!_connected && setup::CONNECT_SMI_IVIEWX)
		{
			ConnectEyeTracker("SMIiViewXPlugin");
		}

		// Try to load Visual Interaction myGaze plugin
		if (!_connected && setup::CONNECT_VI_MYGAZE)
		{
			ConnectEyeTracker("VImyGazePlugin");
		}

		// Try to load Tobii EyeX plugin
		if (!_connected && setup::CONNECT_TOBII_EYEX)
		{
			ConnectEyeTracker("TobiiEyeXPlugin");
		}

#endif

		// If not connected to any eye tracker, provide feedback
		if (!_connected)
		{
			LogInfo("EyeInput: No eye tracker connected. Input emulated by mouse.");
			_statusCallback(EyeInput::Status::DISCONNECTED);
		}
		else
		{
			_statusCallback(EyeInput::Status::CONNECTED);
		}
	}));
}

EyeInput::~EyeInput()
{

#ifdef _WIN32

	// First, wait for eye tracker connection thread to join
	LogInfo("EyeInput: Make sure that eye tracker connection thread is joined.");
	_upConnectionThread->join();

	// Check whether necessary to disconnect
	if (_pluginHandle != NULL)
	{
		// Disconnect eye tracker if necessary
		if (_connected)
		{
			typedef bool(__cdecl *DISCONNECT)();
			DISCONNECT procDisconnect = (DISCONNECT)GetProcAddress(_pluginHandle, "Disconnect");

			// Disconnect eye tracker when procedure available
			if (procDisconnect != NULL)
			{
				LogInfo("EyeInput: About to disconnect eye tracker.");
				bool result = procDisconnect();

				// Check whether disconnection has been successful
				if (result)
				{
					LogInfo("EyeInput: Disconnecting eye tracker successful.");
				}
				else
				{
					LogInfo("EyeInput: Disconnecting eye tracker failed.");
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

	// Update gaze by eye tracker
	double filteredGazeX = 0;
	double filteredGazeY = 0;

	// Bool whether eye tracker is tracking
	bool isTracking = false;

#ifdef _WIN32

	if (_connected && _procFetchGaze != NULL && _procIsTracking != NULL)
	{
		// Prepare vectors to fill
		std::vector<std::pair<double, double> > gazeSamples;

		// Fetch k or less valid samples
		_procFetchGaze(EYETRACKER_AVERAGE_SAMPLE_COUNT, gazeSamples);

		// Convert parameters to double (use same values for all samples,
		// although they could have been collected whil windows transformation has been different)
		double windowXDouble = (double)windowX;
		double windowYDouble = (double)windowY;
		double windowWidthDouble = (double)windowWidth;
		double windowHeightDouble = (double)windowHeight;

		// Average the given samples as long as they do not exceed a predefined distance (aka new fixation)
		if (!gazeSamples.empty())
		{
			double sumX = 0;
			double sumY = 0;
			int sampleCount = 0;
			for (std::pair<double, double> gaze : gazeSamples)
			{
				// Do some clamping according to window coordinates for gaze x
				double clampedX = gaze.first - windowXDouble;
				clampedX = clampedX > 0.0 ? clampedX : 0.0;
				clampedX = clampedX < windowWidthDouble ? clampedX : windowWidthDouble;

				// Do some clamping according to window coordinates for gaze y
				double clampedY = gaze.second - windowYDouble;
				clampedY = clampedY > 0.0 ? clampedY : 0.0;
				clampedY = clampedY < windowHeightDouble ? clampedY : windowHeightDouble;

				// Calculate filtered gaze
				filteredGazeX = sumX / sampleCount;
				filteredGazeY = sumY / sampleCount;

				// Check whether new sample is withing same fixation
				if (sampleCount > 0)
				{
					if (glm::distance(
							glm::vec2(filteredGazeX, filteredGazeY),
							glm::vec2(clampedX, clampedY))
						> setup::EYEINPUT_GAZE_FIXATION_RADIUS)
					{
						break;
					}
				}

				// Sum values
				sumX += clampedX;
				sumY += clampedY;

				// Increase sample count
				sampleCount++;
			}

			// Calculate (final) filtered gaze
			filteredGazeX = sumX / sampleCount;
			filteredGazeY = sumY / sampleCount;
		}

		// Check, whether eye tracker is tracking
		isTracking = _procIsTracking();
	}

#endif

	// ### MOUSE INPUT ###

	// Mouse override of eye tracker
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
		!_connected // eye tracker not connected
		|| _mouseOverride // eye tracker overriden by mouse
		|| !isTracking; // eye tracker not available

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

	// Return whether gaze coordinates comes from eye tracker
	return !gazeEmulated;
}

void EyeInput::Calibrate()
{
#ifdef _WIN32
	if (_connected && _procCalibrate != NULL)
	{
		_procCalibrate();
	}
#endif
}