//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "EyeInput.h"
#include "src/Utils/Logger.h"
#include <cmath>

#ifdef SMI_SUPPORT
#include "src/Input/Eyetracker/SMI.h"
#endif
#ifdef TOBII_SUPPORT
#include "src/Input/Eyetracker/Tobii.h"
#endif

EyeInput::EyeInput(bool useEmulation)
{
    // Only try to connect when no emulation
    if(!useEmulation)
    {
#ifdef SMI_SUPPORT
        _upEyetracker = std::unique_ptr<Eyetracker>(new SMI);
        LogInfo("Trying to connect to SMI iViewX...");
        if (!_upEyetracker->Connect())
        {
            _upEyetracker.reset(NULL);
			LogInfo("..failed.");
        }
        else
        {
			LogInfo("..connected.");
        }
#endif
#ifdef TOBII_SUPPORT
        if (_upEyetracker == NULL) // Do not try to use EyeX when SMI connected
        {
			LogInfo("Trying to connect to Tobii EyeX SDK...");
            _upEyetracker = std::unique_ptr<Eyetracker>(new Tobii);
            if (!_upEyetracker->Connect())
            {
                _upEyetracker.reset(NULL);
				LogInfo("..failed.");
            }
            else
            {
				LogInfo("..connected.");
            }
        }
#endif
    }

    // Log when there is no eyetracker
    if (_upEyetracker == NULL)
    {
        LogInfo("Input emulated by mouse.");
    }
}

EyeInput::~EyeInput()
{
    // Disconnect eyetracker if necessary
    if (_upEyetracker != NULL)
    {
        if (_upEyetracker->Disconnect())
        {
            LogInfo("Disconnecting eyetracker successful.");
        }
        else
        {
            LogInfo("Disconnecting eyetracker failed.");
        }
    }
}

bool EyeInput::Update(float tpf, GLFWwindow* pWindow, double& rGazeX, double& rGazeY)
{
    // Get cursor coordinates
    double currentMouseX;
    double currentMouseY;
    glfwGetCursorPos(pWindow, &currentMouseX, &currentMouseY);

    // Mouse override of eyetracker
    if(_mouseOverride)
    {
        // Check whether override should stop
        if(currentMouseX == _mouseX && currentMouseY == _mouseY)
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
                float x = (float)(currentMouseX - _mouseOverrideX);
                float y = (float)(currentMouseY - _mouseOverrideY);
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
            if(currentMouseX != _mouseX || currentMouseY != _mouseY)
            {
                // Start override initialization frame
                _mouseOverrideInitFrame = true;
                _mouseOverrideX = currentMouseX;
                _mouseOverrideY = currentMouseY;
                _mouseOverrideTime = EYEINPUT_MOUSE_OVERRIDE_INIT_FRAME_DURATION;
            }
        }
    }

    // Update eyetracker if available
    if (_upEyetracker != NULL)
    {
        _upEyetracker->Update(tpf);
    }

    // Bool to indicate mouse usage
    bool mouseCursorUsed = _upEyetracker == NULL || _mouseOverride;

    // Update mouse cusor visibility
    if(mouseCursorUsed != _mouseCursorVisible)
    {
        _mouseCursorVisible = mouseCursorUsed;
        SetGLFWCursorVisibility(_mouseCursorVisible, pWindow);
    }

    // Save mouse cursor coordinate in members
    _mouseX = currentMouseX;
    _mouseY = currentMouseY;

    // Return
    if (mouseCursorUsed)
    {
        rGazeX = currentMouseX;
        rGazeY = currentMouseY;
        return false;
    }
    else
    {
        rGazeX = _upEyetracker->GetGazeX();
        rGazeY = _upEyetracker->GetGazeY();
        return true;
    }
}

void EyeInput::SetGLFWCursorVisibility(bool visible, GLFWwindow* pWindow) const
{
    if(visible)
    {
        glfwSetInputMode(pWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    else
    {
        glfwSetInputMode(pWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    }
}
