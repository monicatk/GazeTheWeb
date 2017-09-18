//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Global variables which are common to be changed for different setups. May be
// moved to some ini file or settings, soon.

#ifndef SETUP_H_
#define SETUP_H_

#include "src/Input/Filters/FilterKernel.h"
#include <string>

namespace setup
{	
	// Make modes available at runtime as booleans
#ifdef CLIENT_DEBUG
	static const bool	DEBUG_MODE = true;
#else
	static const bool	DEBUG_MODE = false;
#endif
#ifdef CLIENT_DEMO
	static const bool	DEMO_MODE = true;
#else
	static const bool	DEMO_MODE = false;
#endif

	// Window
	static const bool	FULLSCREEN = false; // does not work in combination with eye tracker calibration
	static const bool	MAXIMIZE_WINDOW = false | DEMO_MODE; // only implemented for Windows
	static const int	INITIAL_WINDOW_WIDTH = 1280;
	static const int	INITIAL_WINDOW_HEIGHT = 720;

	// Control TODO: move connect bools to config file
	static const bool	CONNECT_OPEN_GAZE = false;
	static const bool	CONNECT_SMI_IVIEWX = true;
	static const bool	CONNECT_VI_MYGAZE = false;
	static const bool	CONNECT_TOBII_EYEX = true;
	static const float	DURATION_BEFORE_INPUT = 1.f; // wait one second before accepting input
	static const float	MAX_AGE_OF_USED_GAZE = 1.f; // only accept gaze as input that is not older than one second (TODO: this is not used by filter but by master to determine when to stop taking gaze input as serious)
	static const float	DURATION_BEFORE_SUPER_CALIBRATION = 3.f; // duration until recalibration is offered after receiving no gaze samples
	static const bool	PAUSED_AT_STARTUP = false | DEMO_MODE;
	static const float	LINK_CORRECTION_MAX_PIXEL_DISTANCE = 5.f;
	static const int	TEXT_SELECTION_MARGIN = 4; // area which is selected before / after zoom coordinate in CEFPixels
	static const int	FILTER_GAZE_FIXATION_PIXEL_RADIUS = 20;
	static const bool	EYEINPUT_DISTORT_GAZE = false;
	static const float	EYEINPUT_DISTORT_GAZE_BIAS_X = 64.f; // pixels
	static const float	EYEINPUT_DISTORT_GAZE_BIAS_Y = 32.f; // pixels
	static const int	FILTER_MEMORY_SIZE = 1000; // how many samples are kept in memory of the filters
	static const FilterKernel FILTER_KERNEL = FilterKernel::GAUSSIAN;
	static const int	FILTER_WINDOW_SIZE = 60;
	static const bool	FILTER_USE_OUTLIER_REMOVAL = true;

	// Experiments
	static const std::string	LAB_STREAM_OUTPUT_NAME = "GazeTheWebOutput";
	static const std::string	LAB_STREAM_OUTPUT_SOURCE_ID = std::to_string(CLIENT_VERSION); // use client version as source id
	static const std::string	LAB_STREAM_INPUT_NAME = LAB_STREAM_OUTPUT_NAME; //  "GazeTheWebInput"; // may be set to same value as LAB_STREAM_OUTPUT_NAME to receive own events for debugging purposes
	static const bool			LOG_INTERACTIONS = false;
	static const std::string	FIREBASE_API_KEY = "AIzaSyBoySYE4mQVhrtCB_1TbPsXa86W8_y35Ug"; // API key for our Firebase
	static const std::string	FIREBASE_URL = "https://hellofirebase-2d544.firebaseio.com"; // URL of our Firebase
	static const int			SOCIAL_RECORD_SESSION_DIGIT_COUNT = 6;

	// Other
	static const bool	ENABLE_WEBGL = false; // only on Windows
	static const bool	BLUR_PERIPHERY = false;
	static const float	WEB_VIEW_RESOLUTION_SCALE = 1.f;
	static const unsigned int	HISTORY_MAX_PAGE_COUNT = 100; // maximal length of history
}

#endif // SETUP_H_
