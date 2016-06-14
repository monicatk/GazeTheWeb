//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Global variables which are common to be changed for different setups. May be
// moved to some ini file or settings, soon.

#ifndef SETUP_H_
#define SETUP_H_

namespace setup
{
    static const bool FULLSCREEN = false;
    static const int INITIAL_WINDOW_WIDTH = 1280;
    static const int INITIAL_WINDOW_HEIGHT = 720;
    static const float DURATION_BEFORE_INPUT = 1.f; // wait one second before accepting input
    static const bool PAUSED_AT_STARTUP = false;
    static const bool ENABLE_WEBGL = false; // only on Windows
	static const bool LOG_DEBUG_MESSAGES = false;
}

#endif // SETUP_H_
