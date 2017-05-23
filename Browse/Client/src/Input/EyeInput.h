//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Abstracts input from eyetracker and mouse into general eye input. Does fallback
// to mouse cursor input provided by GLFW when no eyetracker available. Handles
// override of eyetracking input when mouse is moved.
// TODO: when eye tracker is installed, connection always succeeds. So maybe make some config file setting for order or activation of single eye tracking devices

#ifndef EYEINPUT_H_
#define EYEINPUT_H_

#include "src/Global.h"
#include <memory>
#include <vector>
#include <thread>

// Necessary for dynamic DLL loading in Windows
#ifdef _WIN32
#include <windows.h>
typedef void(__cdecl *FETCH_GAZE)(int, std::vector<std::pair<double, double> >&);
typedef bool(__cdecl *IS_TRACKING)();
typedef void(__cdecl *CALIBRATE)();
#endif

class EyeInput
{
public:

	// Enumeration about eye tracker status
	enum class Status
	{
		CONNECTED, DISCONNECTED, TRYING_TO_CONNECT
	};

	// Typdef of callback function
	typedef std::function<void(Status)> StatusCallback;

    // Constructor, starts thread to establish eye tracker connection. Callback called from a different thread!
    EyeInput(StatusCallback callback);

    // Destructor
    virtual ~EyeInput();

    // Update. Returns whether gaze is currently used (or emulation via mouse when false)
	// Taking information about window to enable eye tracking in windowed mode
	bool Update(
		float tpf,
		double mouseX,
		double mouseY,
		double& rGazeX,
		double& rGazeY,
		int windowX,
		int windowY,
		int windowWidth,
		int windowHeight);

	// Calibrate the eye tracking device
	void Calibrate();

private:

	// Thread that connects to eye tracking device
	std::unique_ptr<std::thread> _upConnectionThread;

	// ### Variables written by thread ###
#ifdef _WIN32
	// Handle for plugin
	HINSTANCE _pluginHandle = NULL;

	// Handle to fetch gaze
	FETCH_GAZE _procFetchGaze = NULL;

	// Handle to check tracking
	IS_TRACKING _procIsTracking = NULL;

	// Handle to calibration
	CALIBRATE _procCalibrate = NULL;
#endif

	// Remember whether connection has been established
	bool _connected = false; // indicator whether thread was successful
	
	// ###################################

    // Mouse cursor coordinates
    double _mouseX = 0;
    double _mouseY = 0;

    // Eye tracker overidden by mouse
    bool _mouseOverride = false;

    // To override the eye tracker, mouse must be moved within given timeframe a given distance
    int _mouseOverrideX = 0;
    int _mouseOverrideY = 0;
    float _mouseOverrideTime = EYEINPUT_MOUSE_OVERRIDE_INIT_FRAME_DURATION;
    bool _mouseOverrideInitFrame = false;

	// Callback about eye tracker status
	StatusCallback _statusCallback;
};

#endif // EYEINPUT_H_
