//============================================================================
// Distributed under the Apache License, Version 2.0.
//============================================================================

#include "Input.h"

void input_get_xy(int &x, int &y, GLFWwindow* window) {
#ifdef USEEYETRACKER_IVIEW
    x = (int)eye_tracker_x;
    y = (int)eye_tracker_y;
#else
#ifdef USEEYETRACKER_TOBII
    x = (int)eye_tracker_x;
    y = (int)eye_tracker_y;
#else
    double dx, dy;
    glfwGetCursorPos(window, &dx, &dy);
    x = (int)dx;
    y = (int)dy;
#endif
#endif
}

void input_setup() {
#ifdef USEEYETRACKER_IVIEW
    iview_setup();
#endif
#ifdef USEEYETRACKER_TOBII
    tobii_setup();
#endif
}

void input_disconnect() {
#ifdef USEEYETRACKER_IVIEW
    iview_disconnect();
#endif
#ifdef USEEYETRACKER_TOBII
    tobii_disconnect();
#endif
}
