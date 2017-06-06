//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Abstracts input from eyetracker and mouse into general eye input. Does fallback
// to mouse cursor input provided by GLFW when no eyetracker available. Handles
// override of eyetracking input when mouse is moved.

#ifndef EYEINPUT_H_
#define EYEINPUT_H_

#include "src/Global.h"
#include "src/Master/MasterThreadsafeInterface.h"
#include "src/Input/EyeTrackerStatus.h"
#include "src/Input/Filters/SimpleFilter.h"
#include "src/Input/Input.h"
#include "plugins/Eyetracker/Interface/EyetrackerSampleData.h"
#include <memory>
#include <vector>
#include <thread>

// Necessary for dynamic DLL loading in Windows
#ifdef _WIN32
#include <windows.h>
typedef void(__cdecl *FETCH_SAMPLES)(SampleQueue&);
typedef bool(__cdecl *IS_TRACKING)();
typedef void(__cdecl *CALIBRATE)();
#endif

class EyeInput
{
public:

    // Constructor, starts thread to establish eye tracker connection. Callback called from a different thread!
    EyeInput(MasterThreadsafeInterface* _pMasterThreadsafeInterface);

    // Destructor
    virtual ~EyeInput();

    // Update method fills Input struct and returns it
	std::shared_ptr<Input> Update(
		float tpf,
		double mouseX,
		double mouseY,
		int windowX,
		int windowY,
		int windowWidth,
		int windowHeight);

	// Calibrate the eye tracking device
	void Calibrate();

private:

	// Thread that connects to eye tracking device
	std::unique_ptr<std::thread> _upConnectionThread;

	// ###################################
	// ### Variables written by thread ###
	// ###################################
#ifdef _WIN32
	// Handle for plugin
	HINSTANCE _pluginHandle = NULL; // can be not null but still disconnected

	// Handle to fetch gaze samples
	FETCH_SAMPLES _procFetchGazeSamples = NULL;

	// Handle to check tracking
	IS_TRACKING _procIsTracking = NULL;

	// Handle to calibration
	CALIBRATE _procCalibrate = NULL;
#endif

	// Remember whether connection has been established
	bool _connected = false; // indicator whether thread was successfully finished
	
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

	// Filter of gaze data
	SimpleFilter _filter;
};

#endif // EYEINPUT_H_
