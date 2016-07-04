//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "Master.h"
#include "src/LabStream/LabStream.h"
#include "src/Utils/Helper.h"
#include "src/Utils/Logger.h"
#include "submodules/glfw/include/GLFW/glfw3.h"
#include <functional>

Master::Master(CefMediator* pCefMediator)
{
    // Save members
    _pCefMediator = pCefMediator;

    // ### GLFW AND OPENGL ###

    // Create OpenGL context
    LogInfo("Initializing GLFW...");
    glfwInit();
    LogInfo("..done.");

    // Window mode and size
    GLFWmonitor* usedMonitor = NULL;
    if (setup::FULLSCREEN)
    {
        LogInfo("Fullscreen mode");
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode * mode = glfwGetVideoMode(monitor);
        _width = mode->width;
        _height = mode->height;
        usedMonitor = monitor;
    }
    else
    {
        LogInfo("Windowed mode");
    }

    LogInfo("Creating window...");
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    _pWindow = glfwCreateWindow(_width, _height, "GazeTheWeb - Browse", usedMonitor, NULL);
    glfwMakeContextCurrent(_pWindow);
    glfwSetInputMode(_pWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN); // hide native mouse cursor
    LogInfo("..done.");
    LogInfo("Initializing OpenGL...");
    ogl_LoadFunctions();
    LogInfo("..done.");

    // Log hardware and maximum OpenGL version
    const GLubyte* renderer = glGetString(GL_RENDERER);
    LogInfo("GPU: ", std::string(reinterpret_cast<char const*>(renderer)));
    const GLubyte* version = glGetString(GL_VERSION);
    LogInfo("OpenGL Version: ", std::string(reinterpret_cast<char const*>(version)));

    // VSync
    glfwSwapInterval(1);
#ifdef _WIN32
    // Turn on vertical screen sync under Windows
    // (I.e. it uses the WGL_EXT_swap_control extension)
    typedef BOOL(WINAPI *PFNWGLSWAPINTERVALEXTPROC)(int interval);
    PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = NULL;
    wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
    if (wglSwapIntervalEXT)
        wglSwapIntervalEXT(1);
#endif

    // Register callbacks to GLFW
    static std::function<void(int, int, int, int)> kC = [&](int k, int s, int a, int m) { this->GLFWKeyCallback(k, s, a, m); };
    glfwSetKeyCallback(_pWindow, [](GLFWwindow* window, int k, int s, int a, int m) { kC(k, s, a, m); });

    static std::function<void(int, int, int)> bC = [&](int b, int a, int m) { this->GLFWMouseButtonCallback(b, a, m); };
    glfwSetMouseButtonCallback(_pWindow, [](GLFWwindow* window, int b, int a, int m) { bC(b, a, m); });

    static std::function<void(double, double)> cC = [&](double x, double y) { this->GLFWCursorPosCallback(x, y); };
    glfwSetCursorPosCallback(_pWindow, [](GLFWwindow* window, double x, double y) { cC(x, y); });

    static std::function<void(int, int)> fC = [&](int w, int h) { this->GLFWResizeCallback(w, h); };
    glfwSetFramebufferSizeCallback(_pWindow, [](GLFWwindow* window, int w, int h) { fC(w, h); });

    // ### EYEGUI ###

    // Set content path
    eyegui::setRootFilepath(CONTENT_PATH);

    // Set callbacks
    std::function<void(std::string)> printGUICallback = [&](std::string message) { this->GUIPrintCallback(message); };
    eyegui::setErrorCallback(printGUICallback);
    eyegui::setWarningCallback(printGUICallback);

    // Create GUI
    LogInfo("Creating eyeGUI...");
    eyegui::GUIBuilder guiBuilder;
    guiBuilder.width = _width;
    guiBuilder.height = _height;
    guiBuilder.fontFilepath = "fonts/tauri/Tauri-Regular.ttf";
    guiBuilder.characterSet = eyegui::CharacterSet::US_ENGLISH;
    guiBuilder.localizationFilepath = "localizations/English.leyegui";
    guiBuilder.fontTallSize = 0.07f;
    _pGUI = guiBuilder.construct(); // standard GUI object used everywhere
    _pSuperGUI = guiBuilder.construct(); // GUI which is rendered on top of everything else
    LogInfo("..done.");

    // Load configuration
    eyegui::loadConfig(_pGUI, "configuration/Global.ceyegui");
    eyegui::loadConfig(_pSuperGUI, "configuration/Global.ceyegui");

    // Set resize callback of GUI
    std::function<void(int, int)> resizeGUICallback = [&](int width, int height) { this->GUIResizeCallback(width, height); };
    eyegui::setResizeCallback(_pGUI, resizeGUICallback); // only one GUI needs to callback it. Use standard for it

    // Load dictionary
    _dictonaryId = eyegui::addDictionary(_pGUI, "dictionaries/EnglishUS.dic");

    // ### STATES ###

    // Create states
    _upWeb = std::unique_ptr<Web>(new Web(this, _pCefMediator));
    _upSettings = std::unique_ptr<Settings>(new Settings(this));

    // Set first state
    _currentState = StateType::WEB;
    _upWeb->Activate();

    // ### HOMEPAGE ###
    _upWeb->AddTab("https://duckduckgo.com");

    // ### SUPER LAYOUT ###

    // Load layout (deleted at eyeGUI termination)
    _pSuperLayout = eyegui::addLayout(_pSuperGUI, "layouts/Super.xeyegui", EYEGUI_SUPER_LAYER, true);

    // Button listener
    _spMasterButtonListener = std::shared_ptr<MasterButtonListener>(new MasterButtonListener(this));
    eyegui::registerButtonListener(_pSuperLayout, "pause", _spMasterButtonListener);

    // Initialization
    if(setup::PAUSED_AT_STARTUP)
    {
        eyegui::buttonDown(_pSuperLayout, "pause", true);
    }
    _pausedDimming.setValue(0);

    // Add floating frame on empty layout for cursor
    _pCursorLayout = eyegui::addLayout(_pSuperGUI, "layouts/Empty.xeyegui", EYEGUI_CURSOR_LAYER, true);
    eyegui::setInputUsageOfLayout(_pCursorLayout, false);
    _cursorFrameIndex = eyegui::addFloatingFrameWithBrick(_pCursorLayout, "bricks/Cursor.beyegui", 0, 0, 0, 0, true, false); // will be moved and sized in loop

    // ### INPUT ###
    _upEyeInput = std::unique_ptr<EyeInput>(new EyeInput);

    // ### OTHER ###

    // Time
    _lastTime = glfwGetTime();

	// Connection to LabStreamingLayer
	_upLabStream = std::unique_ptr<LabStream>(new LabStream);
}

Master::~Master()
{
    // Manual destruction of Web. Otherwise there are errors in CEF at shutdown (TODO: understand why)
    _upWeb.reset();

    // Terminate eyeGUI
    eyegui::terminateGUI(_pSuperGUI);
    eyegui::terminateGUI(_pGUI);

    // Terminate GLFW
    glfwTerminate();
}

void Master::Run()
{
    this->Loop();
}

double Master::GetTime() const
{
	return glfwGetTime();
}

void Master::Exit()
{
	glfwSetWindowShouldClose(_pWindow, GL_TRUE);
}

eyegui::Layout* Master::AddLayout(std::string filepath, int layer, bool visible)
{
    // Add layout
    eyegui::Layout* pLayout = eyegui::addLayout(_pGUI, filepath, layer, visible);
    return pLayout;
}

void Master::RemoveLayout(eyegui::Layout* pLayout)
{
    // Remove layout
    eyegui::removeLayout(_pGUI, pLayout);
}

void Master::Loop()
{
    while (!glfwWindowShouldClose(_pWindow))
    {
        // Time per frame
        double currentTime = glfwGetTime();
        float tpf = std::min((float) (currentTime - _lastTime), 0.25f); // everything breaks when tpf too big
        _lastTime = currentTime;

        // Decrement time until input is accepted
        if (_timeUntilInput > 0)
        {
            _timeUntilInput -= tpf;
        }

		// Poll lab streaming layer communication (TODO: testing)
		auto labStreamInput = _upLabStream->Poll();
		for (const std::string& rEvent : labStreamInput)
		{
			LogInfo("Master: LabStreamInput = ", rEvent);
		}	

        // Clearing of buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Get cursor coordinates
        double currentMouseX;
        double currentMouseY;
        glfwGetCursorPos(_pWindow, &currentMouseX, &currentMouseY);

        // Update eye input
        double gazeX;
        double gazeY;
        bool gazeUsed = _upEyeInput->Update(tpf, currentMouseX, currentMouseY, gazeX, gazeY);

        // Update cursor
        eyegui::setVisibilityOfLayout(_pCursorLayout, !gazeUsed, false, true);
        float halfRelativeMouseCursorSize = MOUSE_CURSOR_RELATIVE_SIZE / 2.f;
        eyegui::setPositionOfFloatingFrame(
            _pCursorLayout,
            _cursorFrameIndex,
            (gazeX / _width) - halfRelativeMouseCursorSize,
            (gazeY / _height) - halfRelativeMouseCursorSize);
        eyegui::setSizeOfFloatingFrame(
            _pCursorLayout,
            _cursorFrameIndex,
            MOUSE_CURSOR_RELATIVE_SIZE,
            MOUSE_CURSOR_RELATIVE_SIZE);

        // Pause visualization
        _pausedDimming.update(tpf, !_paused);
        eyegui::setValueOfStyleAttribute(
            _pSuperLayout,
            "pause_background",
            "background-color",
            RGBAToHexString(glm::vec4(0, 0, 0, MASTER_PAUSE_ALPHA * _pausedDimming.getValue())));

        // Input struct for eyeGUI
        eyegui::Input eyeGUIInput;

        // Fill input structure
        eyeGUIInput.instantInteraction = (_leftMouseButtonPressed && !gazeUsed) || (_enterKeyPressed && gazeUsed);
        eyeGUIInput.gazeX = (int)gazeX;
        eyeGUIInput.gazeY = (int)gazeY;

        // Check for focus and time until input
        int focused = glfwGetWindowAttrib(_pWindow, GLFW_FOCUSED);
        if((focused <= 0) // window not focused
            || (_timeUntilInput > 0)) // do not use input, yet
        {
            // TODO: Do it more effeciently (like calling it with NULL instead of reference)
            eyeGUIInput.gazeUsed = true;
        }

        // Update eyeGUI
        eyegui::Input usedEyeGUIInput = eyegui::updateGUI(_pSuperGUI, tpf, eyeGUIInput);

        if(_paused)
        {
            // Do not pipe input to standard GUI if paused
            usedEyeGUIInput.gazeUsed = true; // TODO: null pointer would be nicer
        }
        usedEyeGUIInput = eyegui::updateGUI(_pGUI, tpf, usedEyeGUIInput);

        // Do message loop of CEF
        _pCefMediator->DoMessageLoopWork();

        // Create input struct for own framework
        Input input(usedEyeGUIInput.gazeX, usedEyeGUIInput.gazeY, usedEyeGUIInput.gazeUsed);

        // Update current state (one should use here pointer instead of switch case)
        StateType nextState = StateType::WEB;
        switch (_currentState)
        {
        case StateType::WEB:
            nextState = _upWeb->Update(tpf, input);
            _upWeb->Draw();
            break;
        case StateType::SETTINGS:
            nextState = _upSettings->Update(tpf, input);
            _upSettings->Draw();
            break;
        }

        // Check next state
        if (_currentState != nextState)
        {
            // Deactivate current state
            switch (_currentState)
            {
            case StateType::WEB:
                _upWeb->Deactivate();
                break;
            case StateType::SETTINGS:
                _upSettings->Deactivate();
                break;
            }

            // Activate next state
            switch (nextState)
            {
            case StateType::WEB:
                _upWeb->Activate();
                break;
            case StateType::SETTINGS:
                _upSettings->Activate();
                break;
            }

            // Remember state
            _currentState = nextState;
        }

        // Draw eyeGUI on top
        eyegui::drawGUI(_pGUI);
        eyegui::drawGUI(_pSuperGUI);

        // Reset reminder BEFORE POLLING
        _leftMouseButtonPressed = false;
        _enterKeyPressed = false;

        // Swap front and back buffers and poll events
        glfwSwapBuffers(_pWindow);
        glfwPollEvents();
    }
}

void Master::GLFWKeyCallback(int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        switch (key)
        {
            case GLFW_KEY_ESCAPE: { glfwSetWindowShouldClose(_pWindow, GL_TRUE); break; }
            case GLFW_KEY_TAB:  { eyegui::hitButton(_pSuperLayout, "pause"); break; }
            case GLFW_KEY_ENTER: { _enterKeyPressed = true; }
			case GLFW_KEY_S: { _upLabStream->Send("42");  } // TODO: testing
        }
    }
}

void Master::GLFWMouseButtonCallback(int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        _leftMouseButtonPressed = true;
    }
}

void Master::GLFWCursorPosCallback(double xpos, double ypos)
{
    // Nothing to do, yet
}

void Master::GLFWResizeCallback(int width, int height)
{
    // Save it
    _width = width;
    _height = height;

    // Tell it eyeGUI (which indirect tells its Tabs which tell it CEF...)
    eyegui::resizeGUI(_pGUI, _width, _height);
    eyegui::resizeGUI(_pSuperGUI, _width, _height);

    // Set viewport (can be set here, untouched/rescued by eyeGUI)
    glViewport(0, 0, _width, _height);

    // CEF mediator is told to resize tabs via GUI callback
}

void Master::GUIResizeCallback(int width, int height)
{
    // Tell CEF mediator to resize tabs
    // Has to be done after GUI has resized because size of web view is taken from GUI
    _pCefMediator->ResizeTabs();
}

void Master::GUIPrintCallback(std::string message) const
{
    LogInfo("eyeGUI: ", message);
}

void Master::MasterButtonListener::down(eyegui::Layout* pLayout, std::string id)
{
    if(pLayout == _pMaster->_pSuperLayout)
    {
        _pMaster->_paused = true;
		eyegui::setDescriptionVisibility(_pMaster->_pGUI, eyegui::DescriptionVisibility::VISIBLE);
    }
}

void Master::MasterButtonListener::up(eyegui::Layout* pLayout, std::string id)
{
    if(pLayout == _pMaster->_pSuperLayout)
    {
        _pMaster->_paused = false;
		eyegui::setDescriptionVisibility(_pMaster->_pGUI, eyegui::DescriptionVisibility::ON_PENETRATION); // TODO look up in Settings for set value
    }
}
