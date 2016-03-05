//============================================================================
// Distributed under the MIT License.
// Author: Raphael Menges (https://github.com/raphaelmenges)
//============================================================================

#ifndef EYETRACKER_INPUT_H
#define EYETRACKER_INPUT_H

#include "externals/GLFW/include/GLFW/glfw3.h"
#include "externals/iViewX/include/iViewXAPI.h"
#include "externals/eyeGUI/include/eyeGUI.h"
#include <iostream>

// Some hacky, global variables which has to be set
eyegui::Input input;
GLFWwindow* globalWindow;
int ret_connect;

// Only used on windows with eyetracker ;)
int __stdcall SampleCallbackFunction(SampleStruct sampleData)
{
    // Get gaze input into window coordinates
    int gazeX = (int)sampleData.leftEye.gazeX;
    int gazeY = (int)sampleData.leftEye.gazeY;

    // Zero should not occur, only when eye not available
    if (gazeX == 0 && gazeY == 0)
    {
        // Remain on old coordinates (later: catch this case and pipe information to eyeGUI)
        return 1;
    }

    // Clamp coordinates
    int xpos, ypos;
    glfwGetWindowPos(globalWindow, &xpos, &ypos);
    int width, height;
    glfwGetFramebufferSize(globalWindow, &width, &height);

    gazeX = gazeX - xpos;
    gazeX = gazeX > 0 ? gazeX : 0;
    gazeX = gazeX < width ? gazeX : width;

    gazeY = gazeY - ypos;
    gazeY = gazeY > 0 ? gazeY : 0;
    gazeY = gazeY < height ? gazeY : height;

    // Pipe it to eyeGUI
    input.gazeX = gazeX;
    input.gazeY = gazeY;
    return 1;
}

static void initializeEyetracker()
{
    // Initialize eyetracker
    SystemInfoStruct systemInfoData;
    ret_connect = 0;

    // Connect to iViewX
    ret_connect = iV_Connect("127.0.0.1", 4444, "127.0.0.1", 5555);

    switch (ret_connect)
    {
    case RET_SUCCESS:
        std::cout << "Connection was established successfully" << std::endl;

        // Read out meta data from iViewX
        std::cout << "GetSystemInfo: " << iV_GetSystemInfo(&systemInfoData) << std::endl;
        std::cout << "SystemInfo ETSystem: " << systemInfoData.iV_ETDevice << std::endl;
        std::cout << "SystemInfo iV_Version: " << systemInfoData.iV_MajorVersion << "." << systemInfoData.iV_MinorVersion << "." << systemInfoData.iV_Buildnumber << std::endl;
        std::cout << "SystemInfo API_Version: " << systemInfoData.API_MajorVersion << "." << systemInfoData.API_MinorVersion << "." << systemInfoData.API_Buildnumber << std::endl;
        std::cout << "SystemInfo samplerate: " << systemInfoData.samplerate << std::endl;

        // Start data output via callback function
        // Define a callback function for receiving samples
        iV_SetSampleCallback(SampleCallbackFunction);

        break;
    case ERR_COULD_NOT_CONNECT:
        std::cout << "Connection could not be established" << std::endl;
        break;
    case ERR_WRONG_PARAMETER:
        std::cout << "Wrong Parameter used" << std::endl;
        break;
    default:
        std::cout << "Any other error appeared" << std::endl;
        return;
    }
}

static void terminateEyetracker()
{
    // Disconnect eyetracker
    if (ret_connect == RET_SUCCESS)
    {
        // Disable callbacks
        iV_SetSampleCallback(NULL);
        iV_SetTrackingMonitorCallback(NULL);

        // Disconnect
        std::cout << "iV_Disconnect: " << iV_Disconnect() << std::endl;
    }
}

#endif // EYETRACKER_INPUT_H
