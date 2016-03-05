//============================================================================
// Distributed under the MIT License.
// Author: Raphael Menges (https://github.com/raphaelmenges)
//============================================================================

#ifndef ENTRY_POINT_H
#define ENTRY_POINT_H

#define GLM_FORCE_CXX11

// ### SETTINGS #################
//#define USE_EYETRACKER // Only with connected SMI RED eyetracker for the moment
const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;
const bool FULLSCREEN = false; // Uses operation system resolution
// ##############################

#include "externals/eyeGUI/include/eyeGUI.h"
#include "externals/OGL/gl_core_3_3.h"
#include "externals/GLFW/include/GLFW/glfw3.h"
#include "externals/GLM/glm/glm.hpp"
#include <iostream>

#include "SimpleApp.h"
#include "WebView.h"

#ifdef USE_EYETRACKER
#include "EyetrackerInput.h"
#endif

// Consts
const float SCROLL_SPEED = 25;

// Global variables (uncool)
int windowWidth;
int windowHeight;
int webViewX;
int webViewY;
int webViewWidth;
int webViewHeight;
float webViewYOffset = 0;
bool reloadPage = false;

bool clickMode = false;
float linearZoom = 1;
float clickZoom = 1;
glm::vec2 clickPosition = glm::vec2(0,0);
bool newClick = true;
glm::vec2 clickPositionCenterOffset = glm::vec2(0,0);
bool useAutoScroll = false;
bool instantInteraction = false;

// Global variables
eyegui::GUI* pGUI;
eyegui::Layout* pBrowserLayout;
eyegui::Layout* pAddressInputLayout;
CefRefPtr<SimpleApp> pApp;
std::string nextURL;

// Callback to receive information from eyeGUI
void printCallback(std::string message)
{
    std::cout << message << std::endl;
}

// Resize callback
void resizeCallback(GLFWwindow* window, int width, int height)
{
    // Not used right now because bug in webview resizing
    eyegui::resizeGUI(pGUI, width, height);
    windowWidth = width;
    windowHeight = height;
	reloadPage = true;
}

// Key callback for GLFW
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    else if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
    {
        eyegui::hitButton(pBrowserLayout, "click_mode");
    }
    else if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
    {
        eyegui::toggleGazeVisualizationDrawing(pGUI);
    }
    else if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
    {
        instantInteraction = true;
    }
    else if (key == GLFW_KEY_BACKSPACE && action == GLFW_PRESS)
    {
        eyegui::hitButton(pBrowserLayout, "back");
    }
}

// URL button
class URLButtonListener : public eyegui::ButtonListener
{
public:

    void hit(eyegui::Layout* pLayout, std::string id) {}

    void down(eyegui::Layout* pLayout, std::string id)
    {
        nextURL = "";
        eyegui::setContentOfTextBlock(pAddressInputLayout, "address_preview", "Here appears the URL");
        eyegui::setVisibilityOfLayout(pAddressInputLayout, true, true, true);
    }

    void up(eyegui::Layout* pLayout, std::string id) {}
};

// Address input
class AddressInputListener : public eyegui::KeyboardListener
{
public:

    void virtual keyPressed(eyegui::Layout* pLayout, std::string id, std::u16string value)
    {
        // Not used here
    }

    void virtual keyPressed(eyegui::Layout* pLayout, std::string id, std::string value)
    {
        nextURL += value;

        // Tell preview about it
        eyegui::setContentOfTextBlock(pLayout, "address_preview", nextURL);
    }
};

// Use address button
class UseAddressButtonListener : public eyegui::ButtonListener
{
public:

    void hit(eyegui::Layout* pLayout, std::string id) {}

    void down(eyegui::Layout* pLayout, std::string id)
    {
        // Hide input layout and load given url
        eyegui::setVisibilityOfLayout(pAddressInputLayout, false, false, true);
		if (!nextURL.empty())
		{
			pApp->loadNewURL(nextURL);
		}
    }

    void up(eyegui::Layout* pLayout, std::string id)
    {
    }
};

// Delete address button
class DeleteAddressButtonListener : public eyegui::ButtonListener
{
public:

    void hit(eyegui::Layout* pLayout, std::string id) {}

    void down(eyegui::Layout* pLayout, std::string id)
    {
        if (nextURL.size() > 0)
        {
            nextURL = nextURL.substr(0, nextURL.size() - 1);

            // Tell preview about it
            eyegui::setContentOfTextBlock(pLayout, "address_preview", nextURL);
        }
    }

    void up(eyegui::Layout* pLayout, std::string id)
    {
    }
};

// Auto scroll button
class AutoScrollButtonListener : public eyegui::ButtonListener
{
public:

    void hit(eyegui::Layout* pLayout, std::string id){}

    void down(eyegui::Layout* pLayout, std::string id)
    {
        useAutoScroll = true;
        eyegui::setElementActivity(pLayout, "scroll_up", false, true);
        eyegui::setElementActivity(pLayout, "scroll_down", false, true);
    }

    void up(eyegui::Layout* pLayout, std::string id)
    {
        useAutoScroll = false;
        eyegui::setElementActivity(pLayout, "scroll_up", true, true);
        eyegui::setElementActivity(pLayout, "scroll_down", true, true);
    }
};

// Back button
class BackButtonListener : public eyegui::ButtonListener
{
public:

    void hit(eyegui::Layout* pLayout, std::string id){}

    void down(eyegui::Layout* pLayout, std::string id)
    {
        pApp->goBack();
    }

    void up(eyegui::Layout* pLayout, std::string id){}
};

// Forward button
class ForwardButtonListener : public eyegui::ButtonListener
{
public:

    void hit(eyegui::Layout* pLayout, std::string id){}

    void down(eyegui::Layout* pLayout, std::string id)
    {
        pApp->goForward();
    }

    void up(eyegui::Layout* pLayout, std::string id){}
};

// Click mode button
class ClickModeButtonListener : public eyegui::ButtonListener
{
public:

    void hit(eyegui::Layout* pLayout, std::string id){}

    void down(eyegui::Layout* pLayout, std::string id)
    {
        clickMode = true;
    }

    void up(eyegui::Layout* pLayout, std::string id)
    {
        clickMode = false;
        clickPosition = glm::vec2(0,0);
        clickPositionCenterOffset = glm::vec2(0,0);
        linearZoom = 1;
        clickZoom = 1;
        newClick = true;
    }
};

// Link button
class LinkButtonListener : public eyegui::ButtonListener
{
public:

    void hit(eyegui::Layout* pLayout, std::string id){}

    void down(eyegui::Layout* pLayout, std::string id)
    {
        // Decide which bookmark was chosen
        if(id == "wikipedia")
        {
            pApp->loadNewURL("https://en.wikipedia.org/wiki/Main_Page");
        }
        else if(id == "mamem")
        {
            pApp->loadNewURL("http://www.mamem.eu/");
        }
        else if(id == "google")
        {
            pApp->loadNewURL("https://www.google.de/");
        }
        else
        {
            pApp->loadNewURL("https://www.uni-koblenz-landau.de/de/koblenz/");
        }
    }

    void up(eyegui::Layout* pLayout, std::string id){}
};

// Scrolling up
class ScrollUpSensorListener : public eyegui::SensorListener
{
public:

    void virtual penetrated(eyegui::Layout* pLayout, std::string id, float amount)
    {
        webViewYOffset += amount;
    }
};

// Scrolling down
class ScrollDownSensorListener : public eyegui::SensorListener
{
public:

    void virtual penetrated(eyegui::Layout* pLayout, std::string id, float amount)
    {
        webViewYOffset -= amount;
    }
};


// EntryPoint
void entry(const CefMainArgs& args, const CefSettings& settings, CefRefPtr<SimpleApp> app, void* windows_sandbox_info)
{
    // Console (open under Windows when windowed execution)
#ifdef _WIN32
    if (!FULLSCREEN)
    {
        AllocConsole();
        freopen("conin$", "r", stdin);
        freopen("conout$", "w", stdout);
        freopen("conout$", "w", stderr);
    }
#endif

    // Remember app
    pApp = app.get();

    // ### OpenGL and GLFW (I am the main process of CEF3) ###

    // Window and OpenGL initialization
    glfwInit();

    // Window mode and size
    GLFWmonitor* usedMonitor = NULL;
    if (FULLSCREEN)
    {
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode * mode = glfwGetVideoMode(monitor);
        windowWidth = mode->width;
        windowHeight = mode->height;
        usedMonitor = monitor;
    }
    else
    {
        windowWidth = WINDOW_WIDTH;
        windowHeight = WINDOW_HEIGHT;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Browser", usedMonitor, NULL);

#ifdef USE_EYETRACKER

    // Tell EyetrackerInput about window (sooooo hacky, global variable comes from EyetrackerInput.h)
    globalWindow = window;

    // Intialize stuff
    initializeEyetracker();

    // Hide mouse cursor
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

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

    // Set callbacks for GLFW
    glfwSetFramebufferSizeCallback(window, resizeCallback);
    glfwSetKeyCallback(window, keyCallback);

    // ### eyeGUI ###

    // Set content path
    eyegui::setRootFilepath(CONTENT_PATH);

    // Set callbacks for eyeGUI
    eyegui::setErrorCallback(&printCallback);
    eyegui::setWarningCallback(&printCallback);

    // Create GUI
		eyegui::GUIBuilder guiBuilder;
		guiBuilder.width = windowWidth;
		guiBuilder.height = windowHeight;
		guiBuilder.fontFilepath = "fonts/tauri/Tauri-Regular.ttf";
		guiBuilder.characterSet = eyegui::CharacterSet::GERMANY_GERMAN;
		pGUI = guiBuilder.construct();

    // Add layouts
    pBrowserLayout = eyegui::addLayout(pGUI, "layouts/WebView.xeyegui");
    pAddressInputLayout = eyegui::addLayout(pGUI, "layouts/AddressInput.xeyegui", false);

    // Load configuration for eyeGUI
    eyegui::loadConfig(pGUI, "configs/Configuration.ceyegui");

    // Listener
    auto linkListener = std::shared_ptr<LinkButtonListener>(new LinkButtonListener);
    eyegui::registerButtonListener(pBrowserLayout, "wikipedia", linkListener);
    eyegui::registerButtonListener(pBrowserLayout, "mamem", linkListener);
    eyegui::registerButtonListener(pBrowserLayout, "google", linkListener);
    eyegui::registerButtonListener(pBrowserLayout, "uni_koblenz", linkListener);

    auto scrollUpSensorListener = std::shared_ptr<ScrollUpSensorListener>(new ScrollUpSensorListener);
    eyegui::registerSensorListener(pBrowserLayout, "scroll_up", scrollUpSensorListener);

    auto scrollDownSensorListener = std::shared_ptr<ScrollDownSensorListener>(new ScrollDownSensorListener);
    eyegui::registerSensorListener(pBrowserLayout, "scroll_down", scrollDownSensorListener);

    auto clickModeButtonListener = std::shared_ptr<ClickModeButtonListener>(new ClickModeButtonListener);
    eyegui::registerButtonListener(pBrowserLayout, "click_mode", clickModeButtonListener);

    auto urlButtonListener = std::shared_ptr<URLButtonListener>(new URLButtonListener);
    eyegui::registerButtonListener(pBrowserLayout, "edit_url", urlButtonListener);

    auto addressInputListener = std::shared_ptr<AddressInputListener>(new AddressInputListener);
    eyegui::registerKeyboardListener(pAddressInputLayout, "address_input", addressInputListener);

    auto useAddressButtonListener = std::shared_ptr<UseAddressButtonListener>(new UseAddressButtonListener);
    eyegui::registerButtonListener(pAddressInputLayout, "use_address", useAddressButtonListener);

    auto deleteAddressButtonListener = std::shared_ptr<DeleteAddressButtonListener>(new DeleteAddressButtonListener);
    eyegui::registerButtonListener(pAddressInputLayout, "delete_address", deleteAddressButtonListener);

    auto backButtonListener = std::shared_ptr<BackButtonListener>(new BackButtonListener);
    eyegui::registerButtonListener(pBrowserLayout, "back", backButtonListener);

    auto forwardButtonListener = std::shared_ptr<ForwardButtonListener>(new ForwardButtonListener);
    eyegui::registerButtonListener(pBrowserLayout, "forward", forwardButtonListener);

    auto autoScrollButtonListener = std::shared_ptr<AutoScrollButtonListener>(new AutoScrollButtonListener);
    eyegui::registerButtonListener(pBrowserLayout, "auto_scroll", autoScrollButtonListener);

    // Deactivate inactive buttons
    eyegui::setElementActivity(pBrowserLayout, "magnify", false);
    eyegui::setElementActivity(pBrowserLayout, "pivot_mode", false);
    eyegui::setElementActivity(pBrowserLayout, "mouse_mode", false);
    eyegui::setElementActivity(pBrowserLayout, "mark_bookmark", false);
    eyegui::setElementActivity(pBrowserLayout, "scroll_left", false);
    eyegui::setElementActivity(pBrowserLayout, "scroll_right", false);

    // Initial update of eyeGUI to determine webview size
    eyegui::updateGUI(pGUI, 0, eyegui::Input());
    auto innerCellTransformation = eyegui::getAbsolutePositionAndSizeOfElement(pBrowserLayout, "inner_cell");
    auto lowerPanelTransformation = eyegui::getAbsolutePositionAndSizeOfElement(pBrowserLayout, "lower_panel");
    webViewX = innerCellTransformation.x;
    webViewY = innerCellTransformation.y;
    webViewWidth = innerCellTransformation.width;
    webViewHeight = innerCellTransformation.height + lowerPanelTransformation.height;

    // ### WebView ###

    WebView webView(
        webViewX,
        webViewY,
        webViewWidth,
        webViewHeight);

    pApp->SetStuff(webView.getTextureHandle(), &webViewWidth, &webViewHeight);

    // ### CEF ###

    // Initialize CEF AFTER texture creation
    CefInitialize(args, settings, pApp.get(), windows_sandbox_info);

    // ### Loop ###

    // Prepare delta time calculation
    float lastTime, deltaTime;
    lastTime = (float)glfwGetTime();

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Calculate delta time per frame
        float currentTime = (float)glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;


#ifndef USE_EYETRACKER
        // Get current mouse cursor position and give it to eyeGUI as gaze
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        eyegui::Input input;
        input.gazeX = (int)x;
        input.gazeY = (int)y;
#endif

        // Instant interaction
        if(instantInteraction)
        {
            input.instantInteraction = true;
            instantInteraction = false;
        }

        // Update GUI
        eyegui::Input usedInput = eyegui::updateGUI(pGUI, deltaTime, input);

        // Create mouse event
        CefMouseEvent event;

        // Automatic scrolling (should depend on screen resolution...)
        if(useAutoScroll  && !usedInput.gazeUsed && !clickMode)
        {
            double value = - input.gazeY + (windowHeight/2);
            bool negativeValue = value < 0 ? true : false;
            value = value * value;
            webViewYOffset = (float)(0.000004f * value);
            webViewYOffset = negativeValue ? -webViewYOffset : webViewYOffset;
        }

        // Tell browser about scrolling
        if(webViewYOffset != 0)
        {
            pApp->SendMouseWheelEvent(event, 0, SCROLL_SPEED * webViewYOffset);
            webViewYOffset = 0;
        }

        // Give OpenGL the window resolution
        glViewport(0, 0, windowWidth, windowHeight);

        // Clearing of buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render browser
        CefDoMessageLoopWork();

        // Calculate sizes
        innerCellTransformation = eyegui::getAbsolutePositionAndSizeOfElement(pBrowserLayout, "inner_cell");
        lowerPanelTransformation = eyegui::getAbsolutePositionAndSizeOfElement(pBrowserLayout, "lower_panel");
        webViewX = innerCellTransformation.x;
        webViewY = innerCellTransformation.y;
        webViewWidth = innerCellTransformation.width;
        webViewHeight = innerCellTransformation.height + lowerPanelTransformation.height;

        // Update webview
        webView.setSize(webViewWidth, webViewHeight);
        webView.setPosition(webViewX, windowHeight - webViewY - webViewHeight);

		// Check whether page reload should be performed (after window size change)
		if (reloadPage)
		{
			// TODO: does work but size is not correct :(
			pApp->reload();
			reloadPage = false;
		}

        // Click mode
        if(clickMode && !usedInput.gazeUsed)
        {
            // Zooming
            float zoomSpeed = 0.5f;

            // New position of click in web view space
            glm::vec2 newClickPosition = glm::vec2(
                ((float)input.gazeX - webViewX) / webViewWidth,
                ((float)input.gazeY - webViewY) / webViewHeight)
                + clickPositionCenterOffset; // Correction of center directed movement

            // Update click position
            if(!newClick)
            {
                // Delta of new position
                glm::vec2 delta = newClickPosition - clickPosition;

                // The bigger the distance, the slower the zoom
                float speedWeight =  1.0f - (abs(glm::length(delta)) / std::sqrt(webViewWidth*webViewWidth + webViewHeight*webViewHeight));
                zoomSpeed *= speedWeight * speedWeight;

                // Move to new click position (weighted by zoom level for more smoohtness at higher zoom)
                float positionInterpolationSpeed = 5;
                clickPosition = clickPosition + delta * std::min(1.0f, clickZoom * positionInterpolationSpeed * deltaTime);
            }
            else
            {
                // First frame of new click
                clickPosition = newClickPosition;
                zoomSpeed = 0.0f;
                newClick = false;
            }

            // Calculate zoom
            linearZoom += deltaTime * zoomSpeed;
            clickZoom = 1.0f - std::log(linearZoom); // Make zoom better with log function

            // Click position to center offset
            float clickPositionCenterOffsetWeight = 0.5f;
            clickPositionCenterOffset = clickPositionCenterOffsetWeight * (1.0f - clickZoom) * (clickPosition - 0.5f);

            // Check, whether click is done
            if(clickZoom < 0.075f)
            {
                // Fill event with click position in pixel space of web view
                event.x = clickPosition.x * webViewWidth;
                event.y = clickPosition.y * webViewHeight;

                // Click should be performed
                pApp->SendMouseClickEvent(event, false);
                pApp->SendMouseClickEvent(event, true);

                // Reset button and values
                eyegui::buttonUp(pBrowserLayout, "click_mode");
            }
        }

        // Render webview
        webView.draw(windowWidth, windowHeight, clickPositionCenterOffset, clickPosition, clickZoom);

        // Render GUI
        eyegui::drawGUI(pGUI);

        // Swap front and back buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();

        // Reset instant interaction in input structure
        input.instantInteraction = false;
    }

#ifdef USE_EYETRACKER

    terminateEyetracker();

#endif

    // Delete GUI object
    eyegui::terminateGUI(pGUI);

    // Termination of program
    glfwTerminate();
}

#endif // ENTRY_POINT_H
