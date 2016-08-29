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
#include "src/Framebuffer.h"
#include <iostream>

using namespace std;

// Shaders for screen filling rendering
const std::string vertexShaderSource =
"#version 330 core\n"
"void main() {\n"
"}\n";

const std::string geometryShaderSource =
"#version 330 core\n"
"layout(points) in;\n"
"layout(triangle_strip, max_vertices = 4) out;\n"
"out vec2 uv;\n"
"void main() {\n"
"    gl_Position = vec4(1.0, 1.0, 0.0, 1.0);\n"
"    uv = vec2(1.0, 1.0);\n"
"    EmitVertex();\n"
"    gl_Position = vec4(-1.0, 1.0, 0.0, 1.0);\n"
"    uv = vec2(0.0, 1.0);\n"
"    EmitVertex();\n"
"    gl_Position = vec4(1.0, -1.0, 0.0, 1.0);\n"
"    uv = vec2(1.0, 0.0);\n"
"    EmitVertex();\n"
"    gl_Position = vec4(-1.0, -1.0, 0.0, 1.0);\n"
"    uv = vec2(0.0, 0.0);\n"
"    EmitVertex();\n"
"    EndPrimitive();\n"
"}\n";

const std::string fragmentShaderSource =
"#version 330 core\n"
"in vec2 uv;\n"
"out vec4 fragColor;\n"
"uniform sampler2D tex;\n"
"void main() {\n"
"   fragColor = texture(tex, uv);\n"
"}\n";

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
    #ifdef _WIN32
        bool console = false;
        for (int i = 0; i < argc; i++) {
            cout << "argv[" << i << "] = " << argv[i] << endl;
            std::string arg = argv[i];
            if (arg.compare("-console") == 0) {
                console = true;
            }
        }

        if (console) {
			AllocConsole();
			freopen("conin$", "r", stdin);
			freopen("conout$", "w", stdout);
			freopen("conout$", "w", stderr);
        }
    #endif

    // GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// Determine screen resolution
	const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	int resX = mode->width;
	int resY = mode->height;

	// Create window
    GLFWwindow* window = glfwCreateWindow(resX, resY, "GazeTheWeb - Tweet", glfwGetPrimaryMonitor(), NULL);
    glfwMakeContextCurrent(window);

	// OpenGL initialization
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

	// Create framebuffer into which is the whole application rendered since it has to be always 1280x720
	Framebuffer framebuffer(1280, 800);
	framebuffer.Bind();
	framebuffer.AddAttachment(Framebuffer::ColorFormat::RGB, true);
	framebuffer.Unbind();

	// Screen filling quad

	// Vertex shader
	int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	char const * pVertSource = vertexShaderSource.c_str();
	glShaderSource(vertexShader, 1, &pVertSource, NULL);
	glCompileShader(vertexShader);

	// Geometry shader
	int geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
	char const * pGeomSource = geometryShaderSource.c_str();
	glShaderSource(geometryShader, 1, &pGeomSource, NULL);
	glCompileShader(geometryShader);

	// Fragment shader
	int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	char const * pFragSource = fragmentShaderSource.c_str();
	glShaderSource(fragmentShader, 1, &pFragSource, NULL);
	glCompileShader(fragmentShader);

	// Create program
	GLuint screenfillingProgram  = glCreateProgram();
	glAttachShader(screenfillingProgram, vertexShader);
	glAttachShader(screenfillingProgram, geometryShader);
	glAttachShader(screenfillingProgram, fragmentShader);
	glLinkProgram(screenfillingProgram);

	// Delete shaders
	glDeleteShader(vertexShader);
	glDeleteShader(geometryShader);
	glDeleteShader(fragmentShader);

	// Vertex array object
	GLuint screenfillingVAO;
	glGenVertexArrays(1, &screenfillingVAO);

	// Hide mouse for eyetracking input
#if defined(USEEYETRACKER_IVIEW) || defined(USEEYETRACKER_TOBII)
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN); // hide native mouse cursor
#endif

	// Setup input
    input_setup();

	// Prepare delta time
	float lastTime, deltaTime;
	lastTime = (float)glfwGetTime();

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

		// Since the input coordinates are probably in full screen resolution, calculate them into coordinates of framebuffer
		input.gazeX = (int)((float)input.gazeX * (1280.f / (float)resX));
		input.gazeY = (int)((float)input.gazeY * (800.f / (float)resY));

        // Render GUI into framebuffer
		framebuffer.Bind();
        eyegui::Input usedInput = eyegui::updateGUI(login->application->getGUI(), deltaTime, input);
        eyegui::drawGUI(login->application->getGUI());
		framebuffer.Unbind();

		// Render framebuffer on screen
		glBindVertexArray(screenfillingVAO);
		glUseProgram(screenfillingProgram);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, framebuffer.GetAttachment(0));
		glDrawArrays(GL_POINTS, 0, 1);

        // Swap front and back buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    input_disconnect();

    // Delete tmp image folder
    remove_folder(CONTENT_PATH + std::string("/img/tmp"));

    //Destructor
    delete login;

	// Delete screen filling stuff
	glDeleteProgram(screenfillingProgram);
	glDeleteVertexArrays(1, &screenfillingVAO);

    // Termination of program
    glfwTerminate();
    return 0;
}
