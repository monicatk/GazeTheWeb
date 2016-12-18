//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "EyeInput.h"
#include "src/Utils/Logger.h"

#include <cmath>

EyeInput::EyeInput(bool useEmulation)
{

#ifdef _WIN32

	if (!useEmulation)
	{
		// Define procedure signature for connection
		typedef bool(__cdecl *CONNECT)();

		// Try to load SMI iViewX plugin
		if (!_connected)
		{
			_pluginHandle = LoadLibrary(TEXT("SMIiViewXPlugin.dll"));

			// Try to connect to eyetracker
			if (_pluginHandle != NULL)
			{
				LogInfo("EyeInput: Loaded SMIiViewPlugin.");

				// Fetch procedure for connecting
				CONNECT procConnect = (CONNECT)GetProcAddress(_pluginHandle, "Connect");

				// Fetch procedure for fetching gaze data
				_procFetchGaze = (FETCH_GAZE)GetProcAddress(_pluginHandle, "GetKOrLessValidRawGazeEntries");

				// Check whether procedures could be loaded
				if (procConnect != NULL && _procFetchGaze != NULL)
				{
					_connected = procConnect();
					if (_connected)
					{
						LogInfo("EyeInput: Connecting eyetracker successful.");
					}
					else
					{
						_procFetchGaze = NULL;
					}
				}
			}
		}

		// Ok, SMI iViewX could not be connected. Try Tobii EyeX
		if (!_connected)
		{
			_pluginHandle = LoadLibrary(TEXT("TobiiEyeXPlugin.dll"));

			// Try to connect to eyetracker
			if (_pluginHandle != NULL)
			{
				LogInfo("EyeInput: Loaded TobiiEyeXPlugin.");

				// Fetch procedure for connecting
				CONNECT procConnect = (CONNECT)GetProcAddress(_pluginHandle, "Connect");

				// Fetch procedure for fetching gaze data
				_procFetchGaze = (FETCH_GAZE)GetProcAddress(_pluginHandle, "GetKOrLessValidRawGazeEntries");

				// Check whether procedures could be loaded
				if (procConnect != NULL && _procFetchGaze != NULL)
				{
					_connected = procConnect();
					if (_connected)
					{
						LogInfo("EyeInput: Connecting eyetracker successful.");
					}
					else
					{
						_procFetchGaze = NULL;
					}
				}
			}
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
    // Mouse override of eyetracker
    if(_mouseOverride)
    {
        // Check whether override should stop
        if(mouseX == _mouseX && mouseY == _mouseY)
        {
            // Check whether override is over
            _mouseOverrideTime -= tpf;
            if(_mouseOverrideTime <= 0)
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
        if(_mouseOverrideInitFrame)
        {
            // Already inside frame of initialization
            _mouseOverrideTime -= tpf;
            if(_mouseOverrideTime <= 0)
            {
                // Check whether mouse cursor movement was enough to trigger override
                float x = (float)(mouseX - _mouseOverrideX);
                float y = (float)(mouseY - _mouseOverrideY);
                float distance = std::sqrt(x*x + y*y);
                if(distance >= EYEINPUT_MOUSE_OVERRIDE_INIT_DISTANCE)
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
            if(mouseX != _mouseX || mouseY != _mouseY)
            {
                // Start override initialization frame
                _mouseOverrideInitFrame = true;
                _mouseOverrideX = mouseX;
                _mouseOverrideY = mouseY;
                _mouseOverrideTime = EYEINPUT_MOUSE_OVERRIDE_INIT_FRAME_DURATION;
            }
        }
    }

	// Update gaze by eyetracker
	double filteredGazeX = 0;
	double filteredGazeY = 0;

#ifdef _WIN32

	if (_procFetchGaze != NULL && _connected)
	{
		// Prepare vectors to fill
		std::vector<double> gazeXSamples, gazeYSamples;

		// Fetch k or less valid samples
		_procFetchGaze(EYETRACKER_AVERAGE_SAMPLE_COUNT, gazeXSamples, gazeYSamples);

		// TODO: maybe use something like queue to collect raw data, clamp it and then add it to a buffer. Then
		// every sample is clamped with the correct (or at least more corresponding) window coordinates. At the moment,
		// All collected samples are clamped with the current window coordinates

		// Convert parameters to double
		double windowXDouble = (double)windowX;
		double windowYDouble = (double)windowY;
		double windowWidthDouble = (double)windowWidth;
		double windowHeightDouble = (double)windowHeight;

		// TODO: queues may be empty! Then devision through 0 occurs
		// Average the given samples
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
		sum = 0;
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

#endif

    // Bool to indicate mouse usage for gaze coordinates
    bool mouseCursorUsed = !_connected || _mouseOverride;

    // Save mouse cursor coordinate in members
    _mouseX = mouseX;
    _mouseY = mouseY;

    // Return whether gaze coordinates comes from eyetracker
    if (mouseCursorUsed)
    {
        rGazeX = mouseX;
        rGazeY = mouseY;
        return false;
    }
    else
    {
        rGazeX = filteredGazeX;
        rGazeY = filteredGazeY;
        return true;
    }
}
