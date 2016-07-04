//============================================================================
// Distributed under the Apache License, Version 2.0.
//============================================================================

#include "externals/eyeGUI-development/include/eyeGUI.h"
#include "externals/OGL/gl_core_3_3.h"
#include "externals/GLFW/include/GLFW/glfw3.h"
#include "src/TwitterApp.h"
#include "src/LoginArea/Login.h"
#include "src/TwitterClient/ImageDownload.h"
#include "src/Input.h"
#include <iostream>

using namespace std;

// Callback to receive information from eyeGUI
void printCallback(std::string message)
{
    std::cout << message << std::endl;
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_ESCAPE: { glfwSetWindowShouldClose(window, GL_TRUE); break; }
		}
	}
}

/**
* Main function
* Supported arguments:
* + "-console" enables the console.
* @param[in] argc amount of arguments
* @param[in] argv array of arguments
*/
int main(int argc, char* argv[]) {
    //Arguments (Windows only)
    bool fullscreen = false;
    #ifdef _WIN32
        bool console = false;
        for (int i = 0; i < argc; i++) {
            cout << "argv[" << i << "] = " << argv[i] << endl;
            std::string arg = argv[i];
            if (arg.compare("-console") == 0) {
                console = true;
            }
            if (arg.compare("-fullscreen") == 0) {
                fullscreen = true;
            }
        }

        if (console) {
			AllocConsole();
			freopen("conin$", "r", stdin);
			freopen("conout$", "w", stdout);
			freopen("conout$", "w", stderr);
        }
    #endif

    // Window and OpenGL initialization
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#ifdef USEEYETRACKER
    GLFWwindow* window = glfwCreateWindow(1280, 800, "GazeTheWeb - Tweet", glfwGetPrimaryMonitor(), NULL);
#else
    GLFWwindow* window = glfwCreateWindow(1280, 800, "GazeTheWeb - Tweet", fullscreen? glfwGetPrimaryMonitor() : NULL, NULL);
#endif
    glfwMakeContextCurrent(window);
    ogl_LoadFunctions();

    // VSync
    glfwSwapInterval(1);
    #ifdef _WIN32
        // Turn on vertical screen sync under Windows.
        // (I.e. it uses the WGL_EXT_swap_control extension)
        typedef BOOL(WINAPI *PFNWGLSWAPINTERVALEXTPROC)(int interval);
        PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = NULL;
        wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
        if (wglSwapIntervalEXT)
            wglSwapIntervalEXT(1);
    #endif

	// Register callbacks
	glfwSetKeyCallback(window, key_callback);

    //Set content path
    eyegui::setRootFilepath(CONTENT_PATH);

    //Set callbacks
    eyegui::setErrorCallback(&printCallback);
    eyegui::setWarningCallback(&printCallback);

    //Variable to switch between active GUI
    eyegui::GUIBuilder guiBuilder;
    guiBuilder.width = 1280;
    guiBuilder.height = 800;
    Login* login = Login::createInstance(1280,800);

    float lastTime, deltaTime;
    lastTime = (float)glfwGetTime();

	// Hide mouse for eyetracking input
#if defined(USEEYETRACKER_IVIEW) || defined(USEEYETRACKER_TOBII)
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN); // hide native mouse cursor
#endif

	// Setup input
    input_setup();

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        if (TwitterApp::getInstance()->terminate)
        {
            glfwDestroyWindow(window);
        }
        // Calculate delta time per frame
        float currentTime = (float)glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // Clearing of buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Get current mouse cursor position and give it to eyeGUI as gaze
        int x, y;
        eyegui::Input input;

        input_get_xy(x, y, window);
        input.gazeX = x;
        input.gazeY = y;
        input.instantInteraction = false;

        // Render GUI
        eyegui::Input usedInput = eyegui::updateGUI(login->application->getGUI(), deltaTime, input);
        eyegui::drawGUI(login->application->getGUI());

        // Swap front and back buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    input_disconnect();

    // Delete tmp image folder
    remove_folder(CONTENT_PATH + std::string("/img/tmp"));

    //Destructor
    delete login;

    // Termination of program
    glfwTerminate();
    return 0;
}
