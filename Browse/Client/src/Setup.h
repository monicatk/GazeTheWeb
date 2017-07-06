//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Global variables which are common to be changed for different setups. May be
// moved to some ini file or settings, soon.

#ifndef SETUP_H_
#define SETUP_H_

#include <string>

namespace setup
{
	// Window
	static const bool FULLSCREEN = false; // does not work in combination with eye tracker calibration
	static const bool MAXIMIZE_WINDOW = false; // only implemented for Windows
	static const int INITIAL_WINDOW_WIDTH = 1280;
	static const int INITIAL_WINDOW_HEIGHT = 720;

	// Control TODO: move connect bools to config file
	static const bool CONNECT_SMI_IVIEWX = false;
	static const bool CONNECT_VI_MYGAZE = false;
	static const bool CONNECT_TOBII_EYEX = true;
	static const float DURATION_BEFORE_INPUT = 1.f; // wait one second before accepting input
	static const float MAX_AGE_OF_USED_GAZE = 1.f; // only accept gaze as input that is not older than one second
	static const float DURATION_BEFORE_SUPER_CALIBRATION = 3.f; // duration until recalibration is offered after receiving no gaze samples
	static const bool PAUSED_AT_STARTUP = false;
	static const float LINK_CORRECTION_MAX_PIXEL_DISTANCE = 5.f;
	static const int TEXT_SELECTION_MARGIN = 4; // area which is selected before / after zoom coordinate in CEFPixels
	static const int FILTER_GAZE_FIXATION_PIXEL_RADIUS = 20;
	static const bool EYEINPUT_DISTORT_GAZE = false;
	static const float EYEINPUT_DISTORT_GAZE_BIAS_X = 64.f; // pixels
	static const float EYEINPUT_DISTORT_GAZE_BIAS_Y = 32.f; // pixels

	// Debugging
	static const bool LOG_DEBUG_MESSAGES = false;
	static const bool DRAW_DEBUG_OVERLAY = false;

	// Experiments
	static const std::string LAB_STREAM_OUTPUT_NAME = "BrowserOutputStream";
	static const std::string LAB_STREAM_OUTPUT_SOURCE_ID = "myuniquesourceid23443";
	static const std::string LAB_STREAM_INPUT_NAME = "MiddlewareStream"; // may be set to same value as LAB_STREAM_OUTPUT_NAME to receive own events for debugging purposes
	static const bool LOG_INTERACTIONS = false;

	// Other
	static const bool ENABLE_WEBGL = false; // only on Windows
	static const bool BLUR_PERIPHERY = false;
	static const float WEB_VIEW_RESOLUTION_SCALE = 1.f;
	static const unsigned int HISTORY_MAX_PAGE_COUNT = 100; // maximal length of history
}

#endif // SETUP_H_
