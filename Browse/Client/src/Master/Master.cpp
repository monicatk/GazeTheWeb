//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "Master.h"
#include "src/Utils/Helper.h"
#include "src/Utils/Logger.h"
#include "src/Arguments.h"
#include "submodules/glfw/include/GLFW/glfw3.h"
#include "submodules/text-csv/include/text/csv/ostream.hpp"
#include <functional>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <string>
#include <fstream>

#ifdef _WIN32 // Windows
// Native access to Windows functions is necessary to maximize window
#define GLFW_EXPOSE_NATIVE_WIN32
#include "submodules/glfw/include/GLFW/glfw3native.h"
#endif

// Namespace for text-csv
namespace csv = ::text::csv;

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

const std::string blurFragmentShaderSource =
"#version 330 core\n"
"const float offset = 1.9;\n" // TODO: maybe use resolution here
"in vec2 uv;\n"
"out vec4 fragColor;\n"
"uniform sampler2D tex;\n"
"uniform vec2 focusPixelPosition;\n"
"uniform float focusPixelRadius;\n"
"uniform float peripheryMultiplier;\n"
"void main() {\n"
// Preparation
"   vec4 color = texture(tex, uv);\n"
"   float mask = min(distance(focusPixelPosition, gl_FragCoord.xy) / focusPixelRadius, 1.0);\n"
"   vec2 texSize = textureSize(tex, 0);\n"
"   vec4 blur = vec4(0,0,0,0);\n"
// x and y not zero
"   for(int x = 1; x <= 2; x++) {\n"
"       for(int y = 1; y <= 2; y++) {\n"
"           blur += texture(tex, (gl_FragCoord.xy + (offset * vec2(x, y))) / texSize);\n"
"           blur += texture(tex, (gl_FragCoord.xy + (offset * vec2(-x, y))) / texSize);\n"
"           blur += texture(tex, (gl_FragCoord.xy + (offset * vec2(x, -y))) / texSize);\n"
"           blur += texture(tex, (gl_FragCoord.xy + (offset * vec2(-x, -y))) / texSize);\n"
"       }\n"
"   }\n"
// x is zero
"   for(int y = 1; y <= 2; y++) {\n"
"       blur += texture(tex, (gl_FragCoord.xy + (offset * vec2(0, y))) / texSize);\n"
"       blur += texture(tex, (gl_FragCoord.xy + (offset * vec2(0, -y))) / texSize);\n"
"   }\n"
// y is zero
"   for(int x = 1; x <= 2; x++) {\n"
"       blur += texture(tex, (gl_FragCoord.xy + (offset * vec2(x, 0))) / texSize);\n"
"       blur += texture(tex, (gl_FragCoord.xy + (offset * vec2(-x, 0))) / texSize);\n"
"   }\n"
// Both is zero
"   blur += color;\n"
// Do composition
"   blur /= 25;\n"
"   color = mix(color, peripheryMultiplier * blur, mask);\n"
"   fragColor = vec4(color.rgb, 1.0);\n"
"}\n";

const std::string simpleFragmentShaderSource =
"#version 330 core\n"
"in vec2 uv;\n"
"out vec4 fragColor;\n"
"uniform sampler2D tex;\n"
"void main() {\n"
"   fragColor = texture(tex, uv);\n"
"}\n";

Master::Master(Mediator* pCefMediator, std::string userDirectory)
{
    // Save members
    _pCefMediator = pCefMediator;
	_userDirectory = userDirectory;

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

	// ### WINDOW ICON ###

	// Set content path (before using it in the helper)
	eyegui::setRootFilepath(CONTENT_PATH);

	// Load window icons for GLFW
	std::vector<unsigned char> icon16Data;
	int icon16Width, icon16Height, icon16ChannelCount;
	eyegui_helper::loadImage("resources/Icon_16.png", icon16Data, icon16Width, icon16Height, icon16ChannelCount);

	std::vector<unsigned char> icon32Data;
	int icon32Width, icon32Height, icon32ChannelCount;
	eyegui_helper::loadImage("resources/Icon_32.png", icon32Data, icon32Width, icon32Height, icon32ChannelCount);

	std::vector<unsigned char> icon64Data;
	int icon64Width, icon64Height, icon64ChannelCount;
	eyegui_helper::loadImage("resources/Icon_64.png", icon64Data, icon64Width, icon64Height, icon64ChannelCount);

	// Create GLFWImages
	std::vector<GLFWimage> icons;
	icons.resize(3);
	icons.at(0).width = icon16Width;
	icons.at(0).height = icon16Height;
	icons.at(0).pixels = icon16Data.data();
	icons.at(1).width = icon32Width;
	icons.at(1).height = icon32Height;
	icons.at(1).pixels = icon32Data.data();
	icons.at(2).width = icon64Width;
	icons.at(2).height = icon64Height;
	icons.at(2).pixels = icon64Data.data();

	// Set window icon
	glfwSetWindowIcon(
		_pWindow,
		(int)icons.size(),
		icons.data());

    // ### EYEGUI ###

	// Decide on localization
	std::string localizationFilepath = "";
	switch (Argument::localization)
	{
	case Argument::Localization::English:
		localizationFilepath = "localizations/English.leyegui";
		break;
	case Argument::Localization::Greek:
		localizationFilepath = "localizations/Greek.leyegui";
		break;
	case Argument::Localization::Hebrew:
		localizationFilepath = "localizations/Hebrew.leyegui";
		break;
	}

    // Set print callbacks
    std::function<void(std::string)> printGUICallback = [&](std::string message) { this->GUIPrintCallback(message); };
    eyegui::setErrorCallback(printGUICallback);
    eyegui::setWarningCallback(printGUICallback);

	// Create GUI builder
	LogInfo("Creating eyeGUI...");
	eyegui::GUIBuilder guiBuilder;
	guiBuilder.width = _width;
	guiBuilder.height = _height;
	guiBuilder.fontFilepath = "fonts/dejavu-sans/ttf/DejaVuSans.ttf";
	guiBuilder.localizationFilepath = localizationFilepath;
	guiBuilder.fontTallSize = 0.07f;
	guiBuilder.useDriftMap = false;

	// Create splash screen GUI, render it one time and throw it away
	eyegui::GUI* pSplashGUI = guiBuilder.construct();
	eyegui::loadStyleSheet(pSplashGUI, "stylesheets/Global.seyegui"); // load styling
	eyegui::addLayout(pSplashGUI, "layouts/Splash.xeyegui");
	eyegui::updateGUI(pSplashGUI, 1.f, eyegui::Input()); // update GUI one time for resizing
	eyegui::drawGUI(pSplashGUI);
	glfwSwapBuffers(_pWindow);
	glfwPollEvents(); // poll events not necessary for GUI but lets display the icon in Windows taskbar earlier
	eyegui::terminateGUI(pSplashGUI);

    // Construct GUI
	guiBuilder.useDriftMap = setup::USE_EYEGUI_DRIFT_MAP;
    _pGUI = guiBuilder.construct(); // standard GUI object used everywhere
	guiBuilder.useDriftMap = false;
    _pSuperGUI = guiBuilder.construct(); // GUI which is rendered on top of everything else
    LogInfo("..done.");

    // Load styling
    eyegui::loadStyleSheet(_pGUI, "stylesheets/Global.seyegui");
	eyegui::loadStyleSheet(_pSuperGUI, "stylesheets/Global.seyegui");

	// Load overriding styles for demo mode
#ifdef CLIENT_DEMO
		eyegui::loadStyleSheet(_pGUI, "stylesheets/Demo.seyegui");
		eyegui::loadStyleSheet(_pSuperGUI, "stylesheets/Demo.seyegui");
#endif

    // Set resize callback of GUI
    std::function<void(int, int)> resizeGUICallback = [&](int width, int height) { this->GUIResizeCallback(width, height); };
    eyegui::setResizeCallback(_pGUI, resizeGUICallback); // only one GUI needs to callback it. Use standard for it

    // Load dictionary
    _dictonaryId = eyegui::addDictionary(_pGUI, "dictionaries/NewEnglishUS.dic");

    // ### INTERACTION LOGGING ###

    // New CSV file for interaction logging
    if(setup::LOG_INTERACTIONS)
    {
        // Name of file
        std::string filename = std::string(INTERACTION_FILE_NAME) + ".csv";

        // Title line
        LogInfo("Create file for interaction logging...");
        LogInfo("File named: ", filename);
        std::ofstream fs(filename, std::ios_base::out); // overwrite existing
        csv::csv_ostream csvs(fs);
        csvs << "Timestamp" << "Layout" << "GazeCoordinate" << "ElementType" << "ElementId" << "ElementRect" << "ElementActivity" << "InteractionType" << "InteractionInfoA";
		fs << std::endl;
        LogInfo("..done.");

        // Listen to eyeGUI
        eyegui::setInteractionCallback([filename](
            std::string layout,
            std::string gazeCoordinate,
            std::string elementType,
            std::string elementId,
            std::string elementRect,
            std::string elementActivity,
            std::string interactionType,
            std::string interactionInfoA)
        {
            // Get current date and time
            const auto currentTime = std::chrono::system_clock::now();
            auto time = std::chrono::system_clock::to_time_t(currentTime);
            std::stringstream ss;
            ss << std::put_time(std::localtime(&time), "%Y-%m-%d %X");

            // Get current miliseconds
            auto currentTimeRounded = std::chrono::system_clock::from_time_t(time);
            if(currentTimeRounded > currentTime)
            {
                --time;
                currentTimeRounded -= std::chrono::seconds(1);
            }
            int milliseconds = std::chrono::duration_cast<std::chrono::duration<int,std::milli> >(currentTime - currentTimeRounded).count();

            // Write everything to file
            std::ofstream fs(filename, std::ios_base::app | std::ios_base::out); // append to existing
            csv::csv_ostream csvs(fs);
			csvs
				<< ss.str() + ":" + std::to_string(milliseconds)
				<< layout
				<< gazeCoordinate
				<< elementType
				<< elementId
				<< elementRect
				<< elementActivity
				<< interactionType
				<< interactionInfoA;
			fs << std::endl;
        });
    }

    // ### STATES ###

    // Create states
    _upWeb = std::unique_ptr<Web>(new Web(this, _pCefMediator, _dataTransfer));
    _upSettings = std::unique_ptr<Settings>(new Settings(this));

    // Set first state
    _currentState = StateType::WEB;
    _upWeb->Activate();

    // ### HOMEPAGE ###
	// _upWeb->AddTab("https://developer.mozilla.org/en-US/docs/Web/CSS/overflow");
	// _upWeb->AddTab("http://html5-demos.appspot.com/static/fullscreen.html");
	// _upWeb->AddTab(std::string(CONTENT_PATH) + "/websites/index.html");
	_upWeb->AddTab(_upSettings->GetHomepage());
	_upWeb->AddTab("http://augreal.mklab.iti.gr/mamem/testing/", false);

    // ### SUPER LAYOUT ###

    // Load layouts (deleted at eyeGUI termination)
    _pSuperLayout = eyegui::addLayout(_pSuperGUI, "layouts/Super.xeyegui", EYEGUI_SUPER_LAYER, true);

	// Load super calibration layout
	_pSuperCalibrationLayout = eyegui::addLayout(_pSuperGUI, "layouts/SuperCalibration.xeyegui", EYEGUI_SUPER_LAYER, false); // adding on top of super layout but still beneath cursor

    // Button listener for pause
    _spMasterButtonListener = std::shared_ptr<MasterButtonListener>(new MasterButtonListener(this));
    eyegui::registerButtonListener(_pSuperLayout, "pause", _spMasterButtonListener);

	// Button listener for super calibration (reusing the master button listener)
	eyegui::registerButtonListener(_pSuperCalibrationLayout, "continue", _spMasterButtonListener);
	eyegui::registerButtonListener(_pSuperCalibrationLayout, "recalibration", _spMasterButtonListener);

	// Add floating frame for notification
	_notificationFrameIndex = eyegui::addFloatingFrameWithBrick(
		_pSuperLayout,
		"bricks/Notification.beyegui",
		(1.f - NOTIFICATION_WIDTH) / 2.f,
		NOTIFICATION_Y,
		NOTIFICATION_WIDTH,
		NOTIFICATION_HEIGHT,
		false,
		false);
	
	// ### CURSOR LAYOUT ###

    // Add floating frame on empty layout for cursor
    _pCursorLayout = eyegui::addLayout(_pSuperGUI, "layouts/Empty.xeyegui", EYEGUI_CURSOR_LAYER, true); // placed over super layer
    eyegui::setInputUsageOfLayout(_pCursorLayout, false);
    _cursorFrameIndex = eyegui::addFloatingFrameWithBrick(_pCursorLayout, "bricks/Cursor.beyegui", 0, 0, 0, 0, true, false); // will be moved and sized in loop

    // ### EYE INPUT ###
	_upEyeInput = std::unique_ptr<EyeInput>(new EyeInput(this, _upSettings->GetEyetrackerGeometry()));

	// ### VOICE INPUT ###
	_upVoiceInput = std::unique_ptr<VoiceInput>(new VoiceInput(_pGUI));

    // ### FRAMEBUFFER ###
    _upFramebuffer = std::unique_ptr<Framebuffer>(new Framebuffer(_width, _height));
	_upFramebuffer->Bind();
    _upFramebuffer->AddAttachment(Framebuffer::ColorFormat::RGB);
	_upFramebuffer->Unbind();
    _upScreenFillingQuad = std::unique_ptr<RenderItem>(
        new RenderItem(
            vertexShaderSource,
            geometryShaderSource,
            setup::BLUR_PERIPHERY ? blurFragmentShaderSource : simpleFragmentShaderSource));

	// ### FIREBASE MAILER ###

	// Login (waits until complete)
	std::promise<std::string> idTokenPromise; auto idTokenFuture = idTokenPromise.get_future(); // future provides initial idToken
	bool pushedBack = FirebaseMailer::Instance().PushBack_Login(_upSettings->GetFirebaseEmail(), _upSettings->GetFirebasePassword(), &idTokenPromise);
	if (pushedBack) { LogInfo(idTokenFuture.get()); };

	// ### JAVASCRIPT TO LAB STREAMING LAYER ###

	// Registers a JavaScript callback function that pipes JS callbacks starting with "lsl:" to LabStreamingLayer
	_pCefMediator->RegisterJavascriptCallback("lsl:", [this](std::string message) { LabStreamMailer::instance().Send(message); });

	// ### JAVASCRIPT TO THIS DATA TRANSFER ###

	// Register callback
	_pCefMediator->RegisterJavascriptCallback("data:", [this](std::string message)
	{
		std::string tag;
		std::string id;
		float x = -1;
		float y = -1;
		auto tokens = SplitBySeparator(message, ',');
		if (tokens.size() > 0) { tag = tokens.at(0); }
		if (tokens.size() > 1) { id = tokens.at(1); }
		if (tokens.size() > 2) { x = std::stof(tokens.at(2)); }
		if (tokens.size() > 3) { y = std::stof(tokens.at(3)); }
		this->NotifyClick(tag, id, x, y);
	});

	// ### LAB STREAM CALLBACK ###

	// Create callback
	_spLabStreamCallback = std::shared_ptr<LabStreamCallback>(new LabStreamCallback(
		[](std::vector<std::string> messages)
		{
			for (const std::string& rMessage : messages)
			{
				// Just print all message received via LabStreamingLayer
				LogInfo("LabStream: " + rMessage);
			}
		}
	));

	// Register callback
	LabStreamMailer::instance().RegisterCallback(_spLabStreamCallback);

	// ### INITIALIZATION ###

	// Paused at startup
	if (setup::PAUSED_AT_STARTUP)
	{
		eyegui::buttonDown(_pSuperLayout, "pause", true);
	}
	_pausedDimming.setValue(0);

	// Show super calibration layout at startup
	if (setup::SUPER_CALIBRATION_AT_STARTUP)
	{
		ShowSuperCalibrationLayout();
	}

    // ### OTHER ###

	// Maximize window if required
#ifdef _WIN32 // Windows
	if (!setup::FULLSCREEN && setup::MAXIMIZE_WINDOW)
	{
		// Fetch handle to window from GLFW
		auto Hwnd = glfwGetWin32Window(_pWindow);

		/*
		// Remove frame from window
		LONG lStyle = GetWindowLong(Hwnd, GWL_STYLE);
		lStyle &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZE | WS_MAXIMIZE | WS_SYSMENU);
		SetWindowLong(Hwnd, GWL_STYLE, lStyle);

		// Do same for extended style
		LONG lExStyle = GetWindowLong(Hwnd, GWL_EXSTYLE);
		lExStyle &= ~(WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);
		SetWindowLong(Hwnd, GWL_EXSTYLE, lExStyle);
		*/

		// Maximize window
		SendMessage(Hwnd, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
	}
#endif

    // Time
    _lastTime = glfwGetTime();
}

Master::~Master()
{
    // Manual destruction of Web. Otherwise there are errors in CEF at shutdown (TODO: understand why)
    _upWeb.reset();

	// Wait for all async jobs to finish
	UpdateAsyncJobs(true);

    // Terminate eyeGUI
    eyegui::terminateGUI(_pSuperGUI);
    eyegui::terminateGUI(_pGUI);

    // Terminate GLFW
    glfwTerminate();
}

bool Master::Run()
{
    this->Loop();
	return _shouldShutdownAtExit;
}

int Master::GetScreenWidth() const
{
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
	return mode->width;
}

int Master::GetScreenHeight() const
{
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
	return mode->height;
}

double Master::GetTime() const
{
    return glfwGetTime();
}

void Master::Exit(bool shutdown)
{
	// Stop update loop
	_exit = true;

	// Should shutdown after exit
	_shouldShutdownAtExit = shutdown;
}

void Master::SetDataTransfer(bool dataTransfer)
{
	// Store value
	_dataTransfer = dataTransfer;

	// Take actions
	if (_dataTransfer)
	{
		// Gaze data recording
		_upEyeInput->ContinueLabStream();

		// FirebaseMailer
		FirebaseMailer::Instance().Continue();

		// Tabs (doing social records etc.)
		_upWeb->SetDataTransfer(true);

		// Visualization
		_upWeb->SetWebPanelMode(WebPanelMode::STANDARD);
		PushNotificationByKey("notification:data_transfer_continued", Type::NEUTRAL, true);

		// Marker in LabStream
		LabStreamMailer::instance().Send("Data transfer continued");

		// TODO: what about local logging?
	}
	else
	{
		// Gaze data recording
		_upEyeInput->PauseLabStream();

		// FirebaseMailer
		FirebaseMailer::Instance().Pause();

		// Tabs (doing no social records etc.)
		_upWeb->SetDataTransfer(false);

		// Visualization
		_upWeb->SetWebPanelMode(WebPanelMode::NO_DATA_TRANSFER);
		PushNotificationByKey("notification:data_transfer_paused", Type::NEUTRAL, true);

		// Marker in LabStream
		LabStreamMailer::instance().Send("Data transfer paused");
	}
}

std::weak_ptr<CustomTransformationInterface> Master::GetCustomTransformationInterface()
{
	return _upEyeInput->GetCustomTransformationInterface();
}

void Master::PushBackAsyncJob(std::function<bool()> job)
{
	// Delegate job into thread
	_asyncJobs.push_back(std::async(
		std::launch::async, // do it asynchronously
		job));
}

void Master::SimplePushBackAsyncJob(FirebaseIntegerKey countKey, FirebaseJSONKey recordKey, nlohmann::json record)
{
	// Add data to record
	record.emplace("startIndex", FirebaseMailer::Instance().GetStartIndex()); // start index
	record.emplace("date", GetDate()); // date

	// Push back the job
	PushBackAsyncJob(
		[countKey, recordKey, record]() // copy of date, start index and success
	{
		// Persist record
		std::promise<int> promise; auto future = promise.get_future(); // future provides index
		bool pushedBack = FirebaseMailer::Instance().PushBack_Transform(countKey, 1, &promise); // adds one to the count
		if (pushedBack) { pushedBack = FirebaseMailer::Instance().PushBack_Put(recordKey, record, std::to_string(future.get() - 1)); } // send JSON to database

		// Return some value (not used)
		return true;
	});
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

std::u16string Master::FetchLocalization(std::string key) const
{
	return eyegui::fetchLocalization(_pGUI, key);
}

void Master::SetStyleTreePropertyValue(std::string styleClass, eyegui::property::Duration type, std::string value)
{
	eyegui::setStyleTreePropertyValue(_pGUI, styleClass, type, value);
}

void Master::SetStyleTreePropertyValue(std::string styleClass, eyegui::property::Color type, std::string value)
{
	eyegui::setStyleTreePropertyValue(_pGUI, styleClass, type, value);
}

void Master::NotifyClick(std::string tag, std::string id, float x, float y)
{
	_upWeb->NotifyClick(tag, id, x, y);
}

void Master::PushNotification(std::u16string content, Type type, bool overridable)
{
	std::string sound;
	switch (type)
	{
	case Type::NEUTRAL:
		// sound = "sounds/GameAudio/TeleportCasual.ogg";
		break;
	case Type::SUCCESS:
		// sound = "sounds/GameAudio/Spacey1upPower-up.ogg";
		break;
	case Type::WARNING:
		// sound = "sounds/GameAudio/SpaceyLoose.ogg";
		break;
	}
	_notificationStack.push(Notification(content, type, overridable, sound));
}

void Master::PushNotificationByKey(std::string key, Type type, bool overridable)
{
	PushNotification(eyegui::fetchLocalization(_pGUI, key), type, overridable);
}

void Master::threadsafe_NotifyEyeTrackerStatus(EyeTrackerStatus status, EyeTrackerDevice device)
{
	// TODO: make job pushing a method of its own
	_threadJobsMutex.lock();
	_threadJobs.push_back(std::make_shared<PushEyetrackerStatusThreadJob>(this, status, device));
	_threadJobsMutex.unlock();
}

bool Master::threadsafe_MayTransferData()
{
	// No job necessary, just reading bool
	return _dataTransfer;
}

void Master::Loop()
{
	while (!_exit)
	{
		// Update the async computations
		UpdateAsyncJobs(false); // do not wait until finished

		// Call exit when window should close
		if (glfwWindowShouldClose(_pWindow))
		{
			Exit();
			continue;
		}

		// Time per frame
		double currentTime = glfwGetTime();
		float tpf = std::min((float)(currentTime - _lastTime), 0.25f); // everything breaks when tpf too big
		_lastTime = currentTime;

		// Decrement time until input is accepted
		if (_timeUntilInput > 0)
		{
			_timeUntilInput -= tpf;
		}

		// Execute thread jobs
		_threadJobsMutex.lock(); // lock jobs
		for (auto& rJob : _threadJobs)
		{
			rJob->Execute();
		}
		_threadJobs.clear();
		_threadJobsMutex.unlock(); // unlock jobs

		// Update lab streaming layer mailer to get incoming messages
		LabStreamMailer::instance().Update();

		// Notification handling
		if (_notificationTime <= 0 // time for the current notification is over
			|| (_notificationOverridable && !_notificationStack.empty())) // go to next notification if current is overridable and stack not empty
		{
			// Show next notification
			if (!_notificationStack.empty())
			{
				// Fetch notification
				auto notification = _notificationStack.front();
				_notificationStack.pop();

				// Set content
				eyegui::setContentOfTextBlock(
					_pSuperLayout,
					"notification",
					notification.message);

				// Decide color of notification
				glm::vec4 color;
				switch (notification.type)
				{
				case(Type::NEUTRAL):
					color = NOTIFICATION_NEUTRAL_COLOR;
					break;
				case(Type::SUCCESS):
					color = NOTIFICATION_SUCCESS_COLOR;
					break;
				case(Type::WARNING):
					color = NOTIFICATION_WARNING_COLOR;
					break;
				}
				
				// Set color in state (TODO: would be better to set / add / remove old style of element so color can be defined in stylesheet)
				eyegui::setStyleTreePropertyValue(_pSuperGUI, "notification", eyegui::property::Color::BackgroundColor, RGBAToHexString(color));

				// Remember whether this notification is overridable
				_notificationOverridable = notification.overridable;

				// Make floating frame visible
				eyegui::setVisibilityOFloatingFrame(_pSuperLayout, _notificationFrameIndex, true, false, true);

				// Reset time
				_notificationTime = NOTIFICATION_DISPLAY_DURATION;

				// Play sound
				if (!notification.sound.empty())
				{
					eyegui::playSound(_pGUI, notification.sound);
				}
			}
			else if(_notificationTime <= 0) // hide notification, if empty and time is over
			{
				// Hide notification display
				eyegui::setVisibilityOFloatingFrame(_pSuperLayout, _notificationFrameIndex, false, false, true);
			}
		}
		else
		{
			_notificationTime -= tpf;
			_notificationTime = glm::max(0.f, _notificationTime);
		}

        // Get cursor coordinates
        double currentMouseX;
        double currentMouseY;
        glfwGetCursorPos(_pWindow, &currentMouseX, &currentMouseY);

		// Update eye input
		int focused = glfwGetWindowAttrib(_pWindow, GLFW_FOCUSED);
		int windowX = 0;
		int windowY = 0;
		glfwGetWindowPos(_pWindow, &windowX, &windowY);
		auto spInput = _upEyeInput->Update(
			focused > 0,
			tpf,
			currentMouseX,
			currentMouseY,
			windowX,
			windowY,
			_width,
			_height); // returns whether gaze was used (or emulated by mouse)

		// If last gaze sample age is too high, perform recalibration
		if (
			!setup::DEMO_MODE // do not do it in DEMO mode
			&& !eyegui::isLayoutVisible(_pSuperCalibrationLayout) // only proceed when layout is not already visible
			&& !spInput->gazeEmulated // only think about calibration if gaze is not emulated
			&& spInput->gazeAge > setup::DURATION_BEFORE_SUPER_CALIBRATION // also only when for given time no samples received
			&& _upEyeInput->SamplesReceived()) // and it should not performed when there were no samples so far
		{
			// Show super calibration layout
			ShowSuperCalibrationLayout();
		}

        // Update cursor with original mouse input
        eyegui::setVisibilityOfLayout(_pCursorLayout, spInput->gazeEmulated, false, true);
        float halfRelativeMouseCursorSize = MOUSE_CURSOR_RELATIVE_SIZE / 2.f;
        eyegui::setPositionOfFloatingFrame(
            _pCursorLayout,
            _cursorFrameIndex,
            (currentMouseX / _width) - halfRelativeMouseCursorSize,
            (currentMouseY / _height) - halfRelativeMouseCursorSize);
        eyegui::setSizeOfFloatingFrame(
            _pCursorLayout,
            _cursorFrameIndex,
            MOUSE_CURSOR_RELATIVE_SIZE,
            MOUSE_CURSOR_RELATIVE_SIZE);

        // Pause visualization
        _pausedDimming.update(tpf, !_paused);
        eyegui::setStyleTreePropertyValue(
			_pSuperGUI,
            "pause_background",
			eyegui::property::Color::BackgroundColor,
            RGBAToHexString(glm::vec4(0, 0, 0, MASTER_PAUSE_ALPHA * _pausedDimming.getValue())));

		// Check whether input is desired
		if ((!spInput->windowFocused) // window not focused
			|| (_timeUntilInput > 0) // do not use input, yet
			|| (!spInput->gazeEmulated && spInput->gazeAge > setup::MAX_AGE_OF_USED_GAZE)) // do not use gaze that is too old
		{
			// TODO: Do it more effeciently (like calling it with NULL instead of reference)
			spInput->gazeUponGUI = true; // means: gaze already consumed, so nothing reacts anymore
		}

        // Fill input structure for eyeGUI
		eyegui::Input eyeGUIInput;
        eyeGUIInput.instantInteraction =
			(_leftMouseButtonPressed && spInput->gazeEmulated) // in case of gaze emulation
			|| (_enterKeyPressed && !spInput->gazeEmulated); // other
        eyeGUIInput.gazeX = (int)spInput->gazeX;
        eyeGUIInput.gazeY = (int)spInput->gazeY;
		eyeGUIInput.gazeUsed = spInput->gazeUponGUI;

        // Update super GUI, including pause button
		eyeGUIInput = eyegui::updateGUI(_pSuperGUI, tpf, eyeGUIInput); // update super GUI with pause button
        if(_paused)
        {
            // Do not pipe input to standard GUI if paused
			eyeGUIInput.gazeUsed = true; // TODO: null pointer would be nicer
        }
		eyeGUIInput = eyegui::updateGUI(_pGUI, tpf, eyeGUIInput); // update GUI

        // Do message loop of CEF
        _pCefMediator->DoMessageLoopWork(); // TODO: Breaks randomly after sometime in debug mode?

        // Update our input structure
		spInput->gazeUponGUI = eyeGUIInput.gazeUsed;
		spInput->instantInteraction = eyeGUIInput.instantInteraction;

        // Bind framebuffer
        _upFramebuffer->Bind();

        // Clearing of buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Disable depth test for drawing
		glDisable(GL_DEPTH_TEST);

        // Update current state and draw it (one should use here pointer instead of switch case)
        StateType nextState = StateType::WEB;
        switch (_currentState)
        {
        case StateType::WEB:
            nextState = _upWeb->Update(tpf, spInput);
            _upWeb->Draw();
            break;
        case StateType::SETTINGS:
            nextState = _upSettings->Update(tpf, spInput);
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

		// Enable depth test again
		glEnable(GL_DEPTH_TEST);

        // Draw eyeGUI on top
        eyegui::drawGUI(_pGUI);
        eyegui::drawGUI(_pSuperGUI);

        // Bind standard framebuffer
        _upFramebuffer->Unbind();

        // Clearing of buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Bind framebuffer as texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, _upFramebuffer->GetAttachment(0));

        // Render screen filling quad
        _upScreenFillingQuad->Bind();

        // Fill uniforms when necessary
        if(setup::BLUR_PERIPHERY)
        {
            _upScreenFillingQuad->GetShader()->UpdateValue("focusPixelPosition", glm::vec2(spInput->gazeX, _height - spInput->gazeY)); // OpenGL coordinate system
            _upScreenFillingQuad->GetShader()->UpdateValue("focusPixelRadius", (float)glm::min(_width, _height) * BLUR_FOCUS_RELATIVE_RADIUS);
            _upScreenFillingQuad->GetShader()->UpdateValue("peripheryMultiplier", BLUR_PERIPHERY_MULTIPLIER);
        }

         _upScreenFillingQuad->Draw(GL_POINTS);

        // Reset reminder BEFORE POLLING
        _leftMouseButtonPressed = false;
        _enterKeyPressed = false;

        // Swap front and back buffers and poll events
        glfwSwapBuffers(_pWindow);
        glfwPollEvents();
    }
}

void Master::UpdateAsyncJobs(bool wait)
{
	// Check asynchronous jobs
	int startIndex = _asyncJobs.size() - 1;
	for (int i = startIndex; i >= 0; i--) // do it from the back
	{
		// Retrieve whether asynchronous call is done
		bool remove = false;
		if (wait)
		{
			_asyncJobs.at(i).wait();
			remove = true; // waited until it is done, so remove it
		}
		else
		{
			// We have no time, so wait for nothing and just check
			if (std::future_status::ready == _asyncJobs.at(i).wait_for(std::chrono::seconds(0)))
			{
				remove = true; // done by now, so remove it
			}
		}

		// Remove future from list when job is done
		if (remove)
		{
			_asyncJobs.erase(_asyncJobs.begin() + i);
		}
	}
}

void Master::ShowSuperCalibrationLayout()
{
	// Display layout to recalibrate
	eyegui::setVisibilityOfLayout(_pSuperCalibrationLayout, true, true, true);

	// Notify user via sound
	eyegui::playSound(_pGUI, "sounds/GameAudio/FlourishSpacey-1.ogg");
}

void Master::GLFWKeyCallback(int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        switch (key)
        {
            case GLFW_KEY_ESCAPE: { Exit(); break; }
            case GLFW_KEY_TAB:  { eyegui::hitButton(_pSuperLayout, "pause"); break; }
            case GLFW_KEY_ENTER: { _enterKeyPressed = true; break; }
			case GLFW_KEY_S: { LabStreamMailer::instance().Send("42"); break; } // TODO: testing
			case GLFW_KEY_R: { ShowSuperCalibrationLayout(); break; } // just show the super calibration layout
			case GLFW_KEY_0: { _pCefMediator->ShowDevTools(); break; }
			case GLFW_KEY_6: { _upWeb->PushBackPointingEvaluationPipeline(PointingApproach::MAGNIFICATION); break; }
			case GLFW_KEY_7: { _upWeb->PushBackPointingEvaluationPipeline(PointingApproach::FUTURE); break; }
			case GLFW_KEY_SPACE: { _upVoiceInput->StartAudioRecording(); break; }
			case GLFW_KEY_M: {

				// Store grid in Firebase
				eyegui::DriftGrid grid = eyegui::getCurrentDriftMap(_pGUI);
				std::vector<float> driftX; driftX.reserve(grid.RES_X + 1);
				std::vector<float> driftY; driftY.reserve(grid.RES_Y + 1);
				for (int x = 0; x <= grid.RES_X; x++)
				{
					for (int y = 0; y <= grid.RES_Y; y++)
					{
						driftX.push_back(grid.verts[x][y].first);
						driftY.push_back(grid.verts[x][y].second);
					}
				}
				nlohmann::json gridJSON =
				{
					{ "resX", grid.RES_X },
					{ "resY", grid.RES_Y },
					{ "driftX", driftX },
					{ "driftY", driftY }
				};
				PushBackAsyncJob(
					[gridJSON]() // provide copy of data
				{
					std::promise<int> promise; auto future = promise.get_future(); // future provides index
					bool pushedBack = FirebaseMailer::Instance().PushBack_Transform(FirebaseIntegerKey::GENERAL_DRIFT_GRID_COUNT, 1, &promise); // adds one to the count
					if (pushedBack) { FirebaseMailer::Instance().PushBack_Put(FirebaseJSONKey::GENERAL_DRIFT_GRID, gridJSON, std::to_string(future.get())); } // send JSON to database
					return true; // give the future some value
				});

				break;
			}
        }
    }
	else if (action == GLFW_RELEASE)
	{
		switch (key)
		{
			case GLFW_KEY_SPACE: { auto voiceAction = _upVoiceInput->EndAndProcessAudioRecording(); LogInfo("Retrieved VoiceAction: ", static_cast<int>(voiceAction)); }
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
    // Independent from bound framebuffer
    glViewport(0, 0, _width, _height);

    // Tell framebuffer about new window size
    _upFramebuffer->Bind();
    _upFramebuffer->Resize(_width, _height);
    _upFramebuffer->Unbind();

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
		_pMaster->SimplePushBackAsyncJob(FirebaseIntegerKey::GENERAL_PAUSE_COUNT, FirebaseJSONKey::GENERAL_PAUSE);
    }
	else if (pLayout == _pMaster->_pSuperCalibrationLayout)
	{
		if(id == "continue")
		{
			// Hide layout
			eyegui::setVisibilityOfLayout(_pMaster->_pSuperCalibrationLayout, false, false, true);
		}
		else if (id == "recalibration")
		{
			// Perform calibration
			bool success = false;
			std::shared_ptr<CalibrationInfo> spCalibrationInfo = std::make_shared<CalibrationInfo>();
			CalibrationResult result = _pMaster->_upEyeInput->Calibrate(spCalibrationInfo);
			switch (result)
			{
			case CALIBRATION_OK:
				_pMaster->PushNotificationByKey("notification:calibration_success", MasterNotificationInterface::Type::SUCCESS, false);
				eyegui::resetDriftMap(_pMaster->_pGUI); // reset drift map of GUI
				success = true;
				break;
			case CALIBRATION_BAD:
				// TODO: provide hints how to improve calibration
				_pMaster->PushNotificationByKey("notification:calibration_bad", MasterNotificationInterface::Type::WARNING, false);
				eyegui::resetDriftMap(_pMaster->_pGUI); // reset drift map of GUI
				success = true;
				break;
			case CALIBRATION_FAILED:
				_pMaster->PushNotificationByKey("notification:calibration_failure", MasterNotificationInterface::Type::WARNING, false);
				break;
			case CALIBRATION_NOT_SUPPORTED:
				_pMaster->PushNotificationByKey("notification:calibration_failure", MasterNotificationInterface::Type::WARNING, false);
				break;
			}

			// Store this recalibration in Firebase
			nlohmann::json record = { { "success", success } };
			_pMaster->SimplePushBackAsyncJob(FirebaseIntegerKey::GENERAL_RECALIBRATION_COUNT, FirebaseJSONKey::GENERAL_RECALIBRATION, record);

			// Remove points of last calibration
			for (const auto& index : _pMaster->_lastCalibrationPointsFrameIndices)
			{
				eyegui::removeFloatingFrame(_pMaster->_pSuperCalibrationLayout, index, false);
			}
			_pMaster->_lastCalibrationPointsFrameIndices.clear();

			// Decide what to display
			if (spCalibrationInfo->empty())
			{
				// Show message
				eyegui::setContentOfTextBlock(_pMaster->_pSuperCalibrationLayout, "calibration_message", eyegui::fetchLocalization(_pMaster->_pSuperGUI, "calibration_message"));
			}
			else
			{
				// Hide message
				eyegui::setContentOfTextBlock(_pMaster->_pSuperCalibrationLayout, "calibration_message", "");

				// Show points of this calibration
				const float calibrationDisplayX = 0.075f;
				const float calibrationDisplayY = 0.175f;
				const float calibrationDisplayWidth = 0.35f;
				const float calibrationDisplayHeight = 0.35f;
				const float calibrationPointSize = 0.1f;
				for (const auto& rPoint : *spCalibrationInfo.get())
				{
					// Decide on point visualization
					std::string brickFilepath = "bricks/CalibrationDisplayOkPoint.beyegui";
					switch (rPoint.result)
					{
					case CALIBRATION_POINT_OK:
						brickFilepath = "bricks/CalibrationDisplayOkPoint.beyegui";
						break;
					case CALIBRATION_POINT_BAD:
						brickFilepath = "bricks/CalibrationDisplayBadPoint.beyegui";
						break;
					case CALIBRATION_POINT_FAILED:
						brickFilepath = "bricks/CalibrationDisplayFailedPoint.beyegui";
						break;
					}

					// Decide on position TODO: this has assumption that calibration is fullscreen on primary display
					float relPointX = (float)rPoint.positionX / _pMaster->GetScreenWidth();
					float relPointY = (float)rPoint.positionY / _pMaster->GetScreenHeight();
					float x = calibrationDisplayX + (relPointX * calibrationDisplayWidth);
					x -= calibrationPointSize / 2.f;
					float y = calibrationDisplayY + (relPointY * calibrationDisplayHeight);
					y -= calibrationPointSize / 2.f;

					// Add point visualization
					_pMaster->_lastCalibrationPointsFrameIndices.push_back(eyegui::addFloatingFrameWithBrick(_pMaster->_pSuperCalibrationLayout, brickFilepath, x, y, calibrationPointSize, calibrationPointSize, true, false));
				}
			}
		}
	}
}

void Master::MasterButtonListener::up(eyegui::Layout* pLayout, std::string id)
{
    if(pLayout == _pMaster->_pSuperLayout)
    {
        _pMaster->_paused = false;
        eyegui::setDescriptionVisibility(_pMaster->_pGUI, eyegui::DescriptionVisibility::ON_PENETRATION); // TODO look up in Settings for set value
		_pMaster->SimplePushBackAsyncJob(FirebaseIntegerKey::GENERAL_UNPAUSE_COUNT, FirebaseJSONKey::GENERAL_UNPAUSE);
    }
}

Master::ThreadJob::~ThreadJob()
{
	// For the sake of C++
}

void Master::PushEyetrackerStatusThreadJob::Execute()
{
	switch (_status)
	{
	case EyeTrackerStatus::TRYING_TO_CONNECT:
		_pMaster->PushNotificationByKey("notification:eye_tracker_status:trying_to_connect", MasterNotificationInterface::Type::NEUTRAL, true);
		break;
	case EyeTrackerStatus::CONNECTED:
		switch (_device)
		{
		case EyeTrackerDevice::OPEN_GAZE:
			_pMaster->PushNotificationByKey("notification:eye_tracker_status:connected_open_gaze", MasterNotificationInterface::Type::SUCCESS, false);
			break;
		case EyeTrackerDevice::SMI_REDN:
			_pMaster->PushNotificationByKey("notification:eye_tracker_status:connected_smi_redn", MasterNotificationInterface::Type::SUCCESS, false);
			break;
		case EyeTrackerDevice::VI_MYGAZE:
			_pMaster->PushNotificationByKey("notification:eye_tracker_status:connected_vi_mygaze", MasterNotificationInterface::Type::SUCCESS, false);
			break;
		case EyeTrackerDevice::TOBII_EYEX:
			_pMaster->PushNotificationByKey("notification:eye_tracker_status:connected_tobii_eyex", MasterNotificationInterface::Type::SUCCESS, false);
			break;
		default:
			_pMaster->PushNotificationByKey("notification:eye_tracker_status:connected", MasterNotificationInterface::Type::SUCCESS, false);
			break;
		}
		
		break;
	case EyeTrackerStatus::DISCONNECTED:
		_pMaster->PushNotificationByKey("notification:eye_tracker_status:disconnected", MasterNotificationInterface::Type::WARNING, false);
		break;
	default:
		// Nothing
		break;
	}
}