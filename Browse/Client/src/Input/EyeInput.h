//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Abstracts input from eyetracker and mouse into general eye input. Does fallback
// to mouse cursor input provided by GLFW when no eyetracker available. Handles
// override of eyetracking input when mouse is moved.

#ifndef EYEINPUT_H_
#define EYEINPUT_H_

#include "src/Input/Eyetracker/Eyetracker.h"
#include "src/Global.h"
#include "externals/GLFW/include/GLFW/glfw3.h"
#include <memory>

class EyeInput
{
public:

    // Constructor
    EyeInput(bool useEmulation = false);

    // Destructor
    virtual ~EyeInput();

    // Update. Needs GLFWwindow to poll cursor coordinates. Returns whether gaze is currently used
    bool Update(float tpf, GLFWwindow* pWindow, double& rGazeX, double& rGazeY);

private:

    // Set GLFW cursor visibility
    void SetGLFWCursorVisibility(bool visible, GLFWwindow* pWindow) const;

    // Vendor of input
    std::unique_ptr<Eyetracker> _upEyetracker = NULL;

    // Mouse cursor coordinates
    double _mouseX = 0;
    double _mouseY = 0;

    // Remember whether mouse cursor is visible
    bool _mouseCursorVisible = true;

    // Eyetracker overidden by mouse
    bool _mouseOverride = false;

    // To override the eyetracker, mouse must be moved within given timeframe a given distance
    int _mouseOverrideX = 0;
    int _mouseOverrideY = 0;
    float _mouseOverrideTime = EYEINPUT_MOUSE_OVERRIDE_INIT_FRAME_DURATION;
    bool _mouseOverrideInitFrame = false;
};

#endif // EYEINPUT_H_
