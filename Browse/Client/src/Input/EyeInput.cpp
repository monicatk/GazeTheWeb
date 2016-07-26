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

    // Update eyetracker if available
    if (_upEyetracker != NULL)
    {
        _upEyetracker->Update(tpf, windowX, windowY, windowWidth, windowHeight);
    }

    // Bool to indicate mouse usage
    bool mouseCursorUsed = _upEyetracker == NULL || _mouseOverride;

    // Save mouse cursor coordinate in members
    _mouseX = mouseX;
    _mouseY = mouseY;

    // Return
    if (mouseCursorUsed)
    {
        rGazeX = mouseX;
        rGazeY = mouseY;
        return false;
    }
    else
    {
        rGazeX = _upEyetracker->GetGazeX();
        rGazeY = _upEyetracker->GetGazeY();
        return true;
    }
}
