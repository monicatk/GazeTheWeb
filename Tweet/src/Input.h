//============================================================================
// Distributed under the Apache License, Version 2.0.
//============================================================================

// This class abstractes from the actual input mode (iViewX or TobiiEyeX or Mouse).
#include "externals/GLFW/include/GLFW/glfw3.h"

#ifdef USEEYETRACKER_IVIEW
    #include "src/Eyetracker/SMI.h"
#endif

#ifdef USEEYETRACKER_TOBII
    #include "src/Eyetracker/Tobii.h"
#endif

// Get the x and y value of the input
void input_get_xy(int &x, int &y, GLFWwindow* window);

// Setup input (only necessary for eye tracker, if mode is mouse nothing will be done here)
void input_setup();

// Disconnect input (only necessary for eye tracker, if mode is mouse nothing will be done here)
void input_disconnect();
