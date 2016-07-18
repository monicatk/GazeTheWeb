//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel M�ller (muellerd@uni-koblenz.de)
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "Tab.h"
#include "src/CEF/Extension/CefMediator.h"
#include "src/State/Web/Tab/WebView.h"
#include "src/Master.h"
#include "src/Global.h"
#include "src/Utils/TabInput.h"
#include "src/Utils/Texture.h"
#include "src/Utils/Helper.h"
#include "src/State/Web/Tab/Pipelines/TestPipeline.h"
#include "src/State/Web/Tab/Pipelines/ZoomClickPipeline.h"
#include "src/Utils/Logger.h"
#include "submodules/glm/glm/gtc/matrix_transform.hpp" // TODO: move to debug rendering class

// Shaders (For debugging)
const std::string vertexShaderSource =
"#version 330 core\n"
"in vec3 posAttr;\n"
"in vec2 uvAttr;\n"
"out vec2 uv;\n"
"uniform mat4 matrix;\n"
"void main() {\n"
"   uv = uvAttr;\n"
"   gl_Position = matrix * vec4(posAttr, 1);\n"
"}\n";

const std::string fragmentShaderSource =
"#version 330 core\n"
"in vec2 uv;\n"
"out vec4 fragColor;\n"
"uniform vec3 color;\n"
"void main() {\n"
"   fragColor = vec4(color,1);\n"
"}\n";

Tab::Tab(Master* pMaster, CefMediator* pCefMediator, std::string url)
{
    // Fill members
    _pMaster = pMaster;
    _pCefMediator = pCefMediator;
    _url = url;

    // Create layouts for Tab (overlay at first, because behind other layouts)
    _pOverlayLayout = _pMaster->AddLayout("layouts/Overlay.xeyegui", EYEGUI_TAB_LAYER, false);
    _pScrollingOverlayLayout = _pMaster->AddLayout("layouts/Overlay.xeyegui", EYEGUI_TAB_LAYER, false);
    _pPanelLayout = _pMaster->AddLayout("layouts/Tab.xeyegui", EYEGUI_TAB_LAYER, false);
    _pPipelineAbortLayout = _pMaster->AddLayout("layouts/TabPipelineAbort.xeyegui", EYEGUI_TAB_LAYER, false);

    // Create scroll up and down floating frames in special overlay layout which always on top of standard overlay
    _scrollUpFrameIndex = eyegui::addFloatingFrameWithBrick(
        _pScrollingOverlayLayout,
        "bricks/TabScrollUpOverlay.beyegui",
        0.5f - (TAB_SCROLLING_SENSOR_WIDTH / 2.f),
        TAB_SCROLLING_SENSOR_PADDING,
        TAB_SCROLLING_SENSOR_WIDTH,
        TAB_SCROLLING_SENSOR_HEIGHT,
        false,
        false);
    _scrollDownFrameIndex = eyegui::addFloatingFrameWithBrick(
        _pScrollingOverlayLayout,
        "bricks/TabScrollDownOverlay.beyegui",
         0.5f - (TAB_SCROLLING_SENSOR_WIDTH / 2.f),
         1.f - TAB_SCROLLING_SENSOR_PADDING - TAB_SCROLLING_SENSOR_HEIGHT,
         TAB_SCROLLING_SENSOR_WIDTH,
         TAB_SCROLLING_SENSOR_HEIGHT,
         false,
         false);

    // Register listener
    _spTabButtonListener = std::shared_ptr<TabButtonListener>(new TabButtonListener(this));
    _spTabSensorListener = std::shared_ptr<TabSensorListener>(new TabSensorListener(this));
    eyegui::registerButtonListener(_pPanelLayout, "click_mode", _spTabButtonListener);
    eyegui::registerButtonListener(_pPanelLayout, "auto_scrolling", _spTabButtonListener);
    eyegui::registerButtonListener(_pPanelLayout, "scroll_to_top", _spTabButtonListener);
    eyegui::registerButtonListener(_pPanelLayout, "zoom", _spTabButtonListener);
    eyegui::registerButtonListener(_pPipelineAbortLayout, "abort", _spTabButtonListener);
    eyegui::registerSensorListener(_pScrollingOverlayLayout, "scroll_up", _spTabSensorListener);
    eyegui::registerSensorListener(_pScrollingOverlayLayout, "scroll_down", _spTabSensorListener);

    // Create listener for overlay
    _spTabOverlayButtonListener = std::shared_ptr<TabOverlayButtonListener>(new TabOverlayButtonListener(this));
    _spTabOverlayKeyboardListener = std::shared_ptr<TabOverlayKeyboardListener>(new TabOverlayKeyboardListener(this));
    _spTabOverlayWordSuggestListener = std::shared_ptr<TabOverlayWordSuggestListener>(new TabOverlayWordSuggestListener(this));

    // Create WebView owned by Tab
    int webRenderWidth, webRenderHeight = 0;
    GetWebRenderResolution(webRenderWidth, webRenderHeight);
    _upWebView = std::unique_ptr<WebView>(new WebView(webRenderWidth, webRenderHeight));

    // Activate current mode
    ActivateMode(_mode);

    // Register itself and painted texture in mediator to receive DOMNodes
    _pCefMediator->RegisterTab(this);

	// Prepare debug rendering
	_upDebugRenderItem = std::unique_ptr<RenderItem>(new RenderItem(vertexShaderSource, fragmentShaderSource));
}

Tab::~Tab()
{
    // Delete triggers before removing layout
    _DOMTriggers.clear();

    _pCefMediator->UnregisterTab(this);
    _pMaster->RemoveLayout(_pPanelLayout);
    _pMaster->RemoveLayout(_pPipelineAbortLayout);
    _pMaster->RemoveLayout(_pOverlayLayout);
    _pMaster->RemoveLayout(_pScrollingOverlayLayout);
}

void Tab::Update(float tpf, Input& rInput)
{
    // ### UPDATE WEB VIEW ###

    // Update WebView (get values from eyeGUI layout directly)
    auto webViewPositionAndSize = CalculateWebViewPositionAndSize();
    _upWebView->Update(
        webViewPositionAndSize.x,
        webViewPositionAndSize.y,
        webViewPositionAndSize.width,
        webViewPositionAndSize.height);

    // ### TAB INPUT STRUCT ###

    // Create tab input structure (like standard input but in addition with input coordinates in web view space)
    int webViewGazeX = rInput.gazeX - webViewPositionAndSize.x;
    int webViewGazeY = rInput.gazeY - webViewPositionAndSize.y;
    float webViewGazeRelativeX = ((float) webViewGazeX) / ((float) webViewPositionAndSize.width);
    float webViewGazeRelativeY = ((float) webViewGazeY) / ((float) webViewPositionAndSize.height);
    TabInput tabInput(
        rInput.gazeX,
        rInput.gazeY,
        rInput.gazeUsed,
        webViewGazeX,
        webViewGazeY,
        webViewGazeRelativeX,
        webViewGazeRelativeY);

    // ### UPDATE COLOR OF GUI ###

    UpdateAccentColor(tpf);

    // ### UPDATE PIPELINES OR MODE ###

    // Decide what to update
    if (_pipelineActive)
    {
        // PIPELINE GETS UPDATED. STANDARD GUI NOT VISIBLE

        // Disable input for panel layout and enable it for pipeline abort layout
        eyegui::setInputUsageOfLayout(_pPanelLayout, false);
        eyegui::setInputUsageOfLayout(_pPipelineAbortLayout, true);

        // Update current pipeline (if there is one)
        if (_pipelines.front()->Update(tpf, tabInput))
        {
            // Remove front element from pipelines
            _pipelines.front()->Deactivate();
            _pipelines.pop_front();

            // Activate new pipeline if there is some
             if (!(_pipelines.empty()))
             {
                _pipelines.front()->Activate();
             }
             else
             {
                // Remember that there is no active pipeline and deactivate GUI to abort it
                SetPipelineActivity(false);
             }
        }
    }
    else
    {
        // MODE GETS UPDATED. STANDARD GUI IS VISIBLE

        // Ask for page resolution
        _timeUntilGetPageResolution -= tpf;
        if (_timeUntilGetPageResolution <= 0)
        {
            _timeUntilGetPageResolution = TAB_GET_PAGE_RES_INTERVAL;
            _pCefMediator->GetPageResolution(this);
        }

        // Enable input for panel layout and disable it for pipeline abort layout
        eyegui::setInputUsageOfLayout(_pPanelLayout, true);
        eyegui::setInputUsageOfLayout(_pPipelineAbortLayout, false);

        // Manual scrolling
        bool showScrollUp = false;
        bool showScrollDown = false;
        if(!_autoScrolling)
        {
            // Scroll up
            if(_scrollingOffsetY > 0)
            {
                showScrollUp = true;
            }

            // Scroll down
            if((_pageHeight-1) > (_scrollingOffsetY + webViewPositionAndSize.height))
            {
                showScrollDown = true;
            }
        }
        eyegui::setVisibilityOFloatingFrame(_pScrollingOverlayLayout, _scrollUpFrameIndex, showScrollUp, false, true);
        eyegui::setVisibilityOFloatingFrame(_pScrollingOverlayLayout, _scrollDownFrameIndex, showScrollDown, false, true);

        // Check, that gaze is not upon a fixed element
        bool gazeUponFixed = false;
        for (const auto& rElements : _fixedElements)
        {
            for (const auto& rElement : rElements)
            {
                // Simple box test (TODO: replace those vectors with nice struct)
                if (tabInput.webViewGazeX > rElement.x
                    && tabInput.webViewGazeY > rElement.y
                    && tabInput.webViewGazeY < rElement.z
                    && tabInput.webViewGazeX < rElement.w)
                {
                    gazeUponFixed = true;
                    break;
                }
            }
            if (gazeUponFixed) { break; }
        }

        // Automatic scrolling. Check whether gaze is available inside webview
        if(_autoScrolling && !tabInput.gazeUsed && tabInput.insideWebView && !gazeUponFixed)
        {
            // Only vertical scrolling supported, yet (TODO: parameters)
            float yOffset = - tabInput.webViewGazeRelativeY + 0.5f;
            yOffset *= 2; // [-1..1]
            bool negative = yOffset < 0;
            yOffset = yOffset * yOffset; // [0..1]
            // yOffset = (glm::max(0.f, yOffset - 0.05f) / 0.95f); // In the center of view no movement
            yOffset = negative ? -yOffset : yOffset; // [-1..1]

            // Update the auto scrolling value (TODO: not frame rate independend)
            _autoScrollingValue += tpf * (yOffset - _autoScrollingValue);
        }
        else if(_autoScrollingValue != 0)
        {
            // Fade it out since auto scrolling still ongoing
            if(_autoScrollingValue > 0)
            {
                _autoScrollingValue -= tpf;
                _autoScrollingValue = glm::max(0.f, _autoScrollingValue);
            }
            else
            {
                _autoScrollingValue += tpf;
                _autoScrollingValue = glm::min(0.f, _autoScrollingValue);
            }
        }

        // Use value of auto scrolling to scroll
        _pCefMediator->EmulateMouseWheelScrolling(this, 0.0, (double)(20.f * _autoScrollingValue));

        // Update triggers
        for(auto& upDOMTrigger : _DOMTriggers)
        {
            upDOMTrigger->Update(tpf, tabInput);
        }

        // Check for mode change
        if(_nextMode != _mode)
        {
            // Deactivate old mode
            DeactivateMode(_mode);
            _mode = _nextMode;
            ActivateMode(_mode);
        }

        // Take a look at current mode
        switch(_mode)
        {
        case TabMode::READ:
            break;
        case TabMode::INTERACTION:
            break;
        case TabMode::CURSOR:
            break;
        }
    }
}

void Tab::Draw() const
{
    // Draw WebView
    _upWebView->Draw(_webViewParameters, _pMaster->GetWindowWidth(), _pMaster->GetWindowHeight());

    // Decide what to draw
    if (_pipelineActive)
    {
        // Draw stuff from current pipeline (overlay etc.)
        _pipelines.front()->Draw();
    }
    else
    {
        // Draw triggers
        for(const auto& upDOMTrigger : _DOMTriggers)
        {
            upDOMTrigger->Draw();
        }

        // Pipeline is empty, so first take a look at current mode
        switch(_mode)
        {
        case TabMode::READ:
            break;
        case TabMode::INTERACTION:
            break;
        case TabMode::CURSOR:
            break;
        }
    }

	// Draw debug overlay
	if (setup::DRAW_DEBUG_OVERLAY)
	{
		DrawDebuggingOverlay();
	}
}

void Tab::Activate()
{
    // Show layouts
    eyegui::setVisibilityOfLayout(_pOverlayLayout, true, true, false);
    eyegui::setVisibilityOfLayout(_pScrollingOverlayLayout, true, true, false);
    eyegui::setVisibilityOfLayout(_pPanelLayout, true, true, false);

    // Setup switches
    if (_autoScrolling) { eyegui::buttonDown(_pPanelLayout, "auto_scrolling", true); }
    if (_zoomLevel != 1.0) { eyegui::buttonDown(_pPanelLayout, "zoom", true); }

    // Screw color interpolation
    _currentColorAccent = _targetColorAccent;
    _colorInterpolation = 1.f;
}

void Tab::Deactivate()
{
    // Hide layouts
    eyegui::setVisibilityOfLayout(_pOverlayLayout, false, true, false);
    eyegui::setVisibilityOfLayout(_pScrollingOverlayLayout, false, true, false);
    eyegui::setVisibilityOfLayout(_pPanelLayout, false, true, false);

    // TODO: THIS SHOULD NOT BE NECESSARY SINCE _pScrollingOverlayLayout IS HIDDEN! WHY?
    eyegui::setVisibilityOFloatingFrame(_pScrollingOverlayLayout, _scrollUpFrameIndex, false, false, true);
    eyegui::setVisibilityOFloatingFrame(_pScrollingOverlayLayout, _scrollDownFrameIndex, false, false, true);

    // Abort pipeline, which also hides GUI for manual abortion
    AbortAndClearPipelines();
}

void Tab::OpenURL(std::string URL)
{
    _url = URL;
    _pCefMediator->RefreshTab(this);
    AbortAndClearPipelines();
}

void Tab::AbortAndClearPipelines()
{
    // Clear pipeline
    _pipelines.clear(); // Destructor of pipeline aborts them
    SetPipelineActivity(false);
}

void Tab::GoForward()
{
    _pCefMediator->GoForward(this);
}

void Tab::GoBack()
{
    _pCefMediator->GoBack(this);
}

void Tab::Reload()
{
    _pCefMediator->ReloadTab(this);
}

void Tab::CalculateWebViewPositionAndSize(int& rX, int& rY, int& rWidth, int& rHeight) const
{
    auto webViewCoordinates = CalculateWebViewPositionAndSize();
    rX = webViewCoordinates.x;
    rY = webViewCoordinates.y;
    rWidth = webViewCoordinates.width;
    rHeight = webViewCoordinates.height;
}

void Tab::GetWindowSize(int& rWidth, int& rHeight) const
{
    rWidth = _pMaster->GetWindowWidth();
    rHeight = _pMaster->GetWindowHeight();
}

int Tab::AddFloatingFrameToOverlay(
    std::string brickFilepath,
    float relativePositionX,
    float relativePositionY,
    float relativeSizeX,
    float relativeSizeY,
    std::map<std::string, std::string> idMapper)
{
    return eyegui::addFloatingFrameWithBrick(
        _pOverlayLayout,
        brickFilepath,
        relativePositionX,
        relativePositionY,
        relativeSizeX,
        relativeSizeY,
        idMapper,
        false,
        false);
}

void Tab::SetPositionOfFloatingFrameInOverlay(
    int index,
    float relativePositionX,
    float relativePositionY)
{
    eyegui::setPositionOfFloatingFrame(_pOverlayLayout, index, relativePositionX, relativePositionY);
}

void Tab::SetVisibilyOfFloatingFrameInOverlay(int index, bool visible)
{
    // Reset when becoming visible
    eyegui::setVisibilityOFloatingFrame(_pOverlayLayout, index, visible, visible, true);
}

void Tab::RemoveFloatingFrameFromOverlay(int index)
{
    eyegui::removeFloatingFrame(_pOverlayLayout, index, true);
}

void Tab::RegisterButtonListenerInOverlay(std::string id, std::function<void(void)> downCallback, std::function<void(void)> upCallback)
{
    // Log bug when id already existing
    auto iter = _overlayButtonDownCallbacks.find(id);
    if (iter != _overlayButtonDownCallbacks.end())
    {
        LogBug("Tab: Element id exists already in overlay button down callbacks: ", id);
    }
    iter = _overlayButtonUpCallbacks.find(id);
    if (iter != _overlayButtonUpCallbacks.end())
    {
        LogBug("Tab: Element id exists already in overlay button up callbacks: ", id);
    }

    _overlayButtonDownCallbacks.emplace(id, downCallback);
    _overlayButtonUpCallbacks.emplace(id, upCallback);

    // Tell eyeGUI about it
    eyegui::registerButtonListener(_pOverlayLayout, id, _spTabOverlayButtonListener);
}

void Tab::UnregisterButtonListenerInOverlay(std::string id)
{
    _overlayButtonDownCallbacks.erase(id);
    _overlayButtonUpCallbacks.erase(id);
}

void Tab::RegisterKeyboardListenerInOverlay(std::string id, std::function<void(std::u16string)> callback)
{
    // Log bug when id already existing
    auto iter = _overlayKeyboardCallbacks.find(id);
    if (iter != _overlayKeyboardCallbacks.end())
    {
        LogBug("Tab: Element id exists already in overlay keyboard callbacks: ", id);
    }

    _overlayKeyboardCallbacks.emplace(id, callback);

    // Tell eyeGUI about it
    eyegui::registerKeyboardListener(_pOverlayLayout, id, _spTabOverlayKeyboardListener);
}

void Tab::UnregisterKeyboardListenerInOverlay(std::string id)
{
    _overlayKeyboardCallbacks.erase(id);
}

void Tab::SetCaseOfKeyboardLetters(std::string id, bool upper)
{
    eyegui::setCaseOfKeyboard(_pOverlayLayout, id, upper ? eyegui::KeyboardCase::UPPER : eyegui::KeyboardCase::LOWER);
}

void Tab::RegisterWordSuggestListenerInOverlay(std::string id, std::function<void(std::u16string)> callback)
{
    // Log bug when id already existing
    auto iter = _overlayWordSuggestCallbacks.find(id);
    if (iter != _overlayWordSuggestCallbacks.end())
    {
        LogBug("Tab: Element id exists already in overlay word suggest callbacks: ", id);
    }

    _overlayWordSuggestCallbacks.emplace(id, callback);

    // Tell eyeGUI about it
    eyegui::registerWordSuggestListener(_pOverlayLayout, id, _spTabOverlayWordSuggestListener);
}

void Tab::UnregisterWordSuggestListenerInOverlay(std::string id)
{
    _overlayWordSuggestCallbacks.erase(id);
}

void Tab::DisplaySuggestionsInWordSuggest(std::string id, std::u16string input)
{
    if(input.empty())
    {
        eyegui::clearSuggestions(_pOverlayLayout, id);
    }
    else
    {
        eyegui::suggestWords(_pOverlayLayout, id, input, _pMaster->GetDictionary());
    }
}

void Tab::GetScrollingOffset(double& rScrollingOffsetX, double& rScrollingOffsetY) const
{
    rScrollingOffsetX = _scrollingOffsetX;
    rScrollingOffsetY = _scrollingOffsetY;
}

void Tab::SetPageResolution(double width, double height)
{
    if (width != _pageWidth || height != _pageHeight)
    {
        LogInfo("Tab: Page size (w: ", _pageWidth, ", h: ", _pageHeight, ") changed to (w: ", width, ", h: ", height, ").");
    }
    _pageWidth = width;
    _pageHeight = height;
}

void Tab::SetFixedElementsCoordinates(int id, std::vector<glm::vec4> elements)
{
    // Assign list of fixed element coordinates to given position
    _fixedElements.resize(id + 1);
    _fixedElements[id] = elements;
}

void Tab::RemoveFixedElement(int id)
{
    if ((int)_fixedElements.size() > id)
    {
        _fixedElements[id].clear();
    }
}

void Tab::SetContentOfTextBlock(std::string id, std::u16string content)
{
    eyegui::setContentOfTextBlock(_pOverlayLayout, id, content);
}

void Tab::PushBackPipeline(std::unique_ptr<Pipeline> upPipeline)
{
    _pipelines.push_back(std::move(upPipeline));
    if (!_pipelineActive)
    {
        // Activate first pipeline
        _pipelines.front()->Activate();

        // Remember that there is active pipeline and activate GUI to abort it
        SetPipelineActivity(true);
    }
}

void Tab::EmulateLeftMouseButtonClick(double x, double y)
{
    _pCefMediator->EmulateLeftMouseButtonClick(this, x, y);
}

void Tab::EmulateMouseWheelScrolling(double deltaX, double deltaY)
{
    _pCefMediator->EmulateMouseWheelScrolling(this, deltaX, deltaY);
}

void Tab::InputTextData(int64 frameID, int nodeID, std::string text, bool submit)
{
    _pCefMediator->InputTextData(this, frameID, nodeID, text, submit);
}

void Tab::GetWebViewTextureResolution(int& rWidth, int& rHeight) const
{
    if (auto spTexture = _upWebView->GetTexture().lock())
    {
        rWidth = spTexture->GetWidth();
        rHeight = spTexture->GetHeight();
    }
    else
    {
        rWidth = 0;
        rHeight = 0;
    }
}

void Tab::GetWebRenderResolution(int& rWidth, int& rHeight) const
{
    auto positionAndSize = CalculateWebViewPositionAndSize();
    rWidth = positionAndSize.width;
    rHeight = positionAndSize.height;
}

void Tab::SetURL(std::string URL)
{
    _url = URL;
}

void Tab::ReceiveFaviconBytes(std::unique_ptr< std::vector<unsigned char> > upData, int width, int height)
{
    // Go over colors and get most colorful as accent (should be always RGBA)
    int size; // width * height * 4
    if (upData != NULL && ((size = (int)upData->size()) >= 4) && width > 0 && height > 0)
    {
        // Show favicon in layout (TODO: test. Name of image must be constructed from tab id)
        // eyegui::setImageOfPicture(_pPanelLayout, "favicon", "test", width, height, eyegui::ColorFormat::RGBA, upData->data(), true);

        // Prepare loop
        int steps = (width * height) / TAB_ACCENT_COLOR_SAMPLING_POINTS;
        steps = glm::max(1, steps);
        int maxIndex = 0;
        int maxSaturation = 0;
        for (int i = 0; i < size; i+= steps * 4)
        {
            // Extract colors
            float r = upData->at(i);
            float g = upData->at(i + 1);
            float b = upData->at(i + 2);
            float a = upData->at(i + 3);

            // Calculate saturation like in HSV color space
            float max = glm::max(r, glm::max(g, b));
            float saturation = 0;
            if (max != 0)
            {
                float delta = max - glm::min(r, glm::min(g, b));
                saturation = delta / max;
                saturation *= a;
            }

            // Is it maximum?
            if (maxSaturation < saturation)
            {
                maxSaturation = saturation;
                maxIndex = i;
            }
        }

        // Extract accent color
        _targetColorAccent = glm::vec4(
            ((float)upData->at(maxIndex) / 255.f),
            ((float)upData->at(maxIndex+1) / 255.f),
            ((float)upData->at(maxIndex+2) / 255.f),
            1.f);

        // Check, whether new target color is too much white
        float sum = _targetColorAccent.r + _targetColorAccent.g + _targetColorAccent.b; // maximal 3
        float whiteBorder = 2.5f;
        if(sum >= whiteBorder)
        {
            // Too bright, darken it
            float multiplier = (1.f - (sum - whiteBorder));
            _targetColorAccent.r *= multiplier;
            _targetColorAccent.g *= multiplier;
            _targetColorAccent.b *= multiplier;
        }
        else if (sum <= 0.3f)
        {
            // Too dark, use default instead
            _targetColorAccent = TAB_DEFAULT_COLOR_ACCENT;
        }
    }
    else
    {
        // Something went wrong at favicon loading
        _targetColorAccent = TAB_DEFAULT_COLOR_ACCENT;
    }

    // Start color accent interpolation
    _colorInterpolation = 0;
}

void Tab::ResetFaviconBytes()
{
    // TODO
}

std::weak_ptr<Texture> Tab::GetWebViewTexture()
{
    return _upWebView->GetTexture();
}

void Tab::AddDOMNode(std::shared_ptr<DOMNode> spNode)
{
    // Create DOMTrigger
    std::unique_ptr<DOMTrigger> upDOMTrigger = std::unique_ptr<DOMTrigger>(new DOMTrigger(this, spNode));

    // Activate trigger
    if (!_pipelineActive)
    {
        upDOMTrigger->Activate();
    }

    // Push it to vector
    _DOMTriggers.push_back(std::move(upDOMTrigger));
}

void Tab::ClearDOMNodes()
{
    // Deactivate all DOMTriggers
    for(auto& upDOMTrigger : _DOMTriggers)
    {
        upDOMTrigger->Deactivate();
    }

    // Clear the vector with them
   _DOMTriggers.clear();
}

void Tab::SetScrollingOffset(double x, double y)
{
    // Update page size information when nearly reached end of page size (in case of endless scrollable sites like e.g. Fb.com)
    // (but don't do it if scrolling offset hasn't changed)
    /*
    auto webViewPositionAndSize = CalculateWebViewPositionAndSize();
    if ((x != _scrollingOffsetX && abs(_pageWidth - webViewPositionAndSize.width - x) < 50)
        || (y != _scrollingOffsetY && abs(_pageHeight - webViewPositionAndSize.height - y) < 50))
    {
        _pCefMediator->GetPageResolution(this);
    }
    */

    _scrollingOffsetX = x;
    _scrollingOffsetY = y;
}

void Tab::SetTitle(std::string title)
{
    _title = title;
}

void Tab::AddTabAfter(std::string URL)
{
    // TODO
    LogDebug("New Tab should be opened: ", URL);
}

eyegui::AbsolutePositionAndSize Tab::CalculateWebViewPositionAndSize() const
{
    return eyegui::getAbsolutePositionAndSizeOfElement(_pPanelLayout, "web_view");
}

void Tab::SetPipelineActivity(bool active)
{
    if(active)
    {
        _pipelineActive = true;
        eyegui::setVisibilityOfLayout(_pPipelineAbortLayout, true, true, true);

        // Deactivate all triggers
        for(auto& upDOMTrigger : _DOMTriggers)
        {
            upDOMTrigger->Deactivate();
        }

        // Deactivate scrolling overlay
        eyegui::setVisibilityOfLayout(_pScrollingOverlayLayout, false, false, true);
    }
    else
    {
        _pipelineActive = false;
        eyegui::setVisibilityOfLayout(_pPipelineAbortLayout, false, false, true);

        // Activate all triggers
        for(auto& upDOMTrigger : _DOMTriggers)
        {
            upDOMTrigger->Activate();
        }

        // Activate scrolling overlay
        eyegui::setVisibilityOfLayout(_pScrollingOverlayLayout, true, true, true);
    }
}

void Tab::UpdateAccentColor(float tpf)
{
    // Move accent color accent towards target accent color
    glm::vec4 colorAccent = _currentColorAccent;
    if (_colorInterpolation < 1.f)
    {
        _colorInterpolation += tpf; // interpolate
        _colorInterpolation = glm::min(_colorInterpolation, 1.f); // clamp
        colorAccent = ((1.f - _colorInterpolation) * _currentColorAccent) + (_colorInterpolation * _targetColorAccent); // calculation of current color
        if (_colorInterpolation == 1.f)
        {
            _currentColorAccent = _targetColorAccent;
        }
    }

    // Create background color
    float backgroundMultiplier = 0.5f;
    float panelBackgroundMultiplier = 0.4f;
    glm::vec4 backgroundAccentColor(
        colorAccent.r * backgroundMultiplier,
        colorAccent.g * backgroundMultiplier,
        colorAccent.b * backgroundMultiplier,
        colorAccent.a);
    glm::vec4 panelBackgroundAccentColor(
        colorAccent.r * panelBackgroundMultiplier,
        colorAccent.g * panelBackgroundMultiplier,
        colorAccent.b * panelBackgroundMultiplier,
        colorAccent.a);

    // Set color of tab_panel style in pipeline abort overlay
    eyegui::setValueOfStyleAttribute(
        _pPipelineAbortLayout,
        "tab_panel",
        "color",
        RGBAToHexString(colorAccent));

    // Set color of tab_panel style in layout
    eyegui::setValueOfStyleAttribute(
        _pPanelLayout,
        "tab_panel",
        "color",
        RGBAToHexString(colorAccent));

    // Set color of tab_overlay style in overlay layout
    eyegui::setValueOfStyleAttribute(
        _pOverlayLayout,
        "tab_overlay",
        "color",
        RGBAToHexString(colorAccent));
    eyegui::setValueOfStyleAttribute(
        _pOverlayLayout,
        "tab_overlay",
        "background-color",
        RGBAToHexString(backgroundAccentColor));

    // Set color of tab_overlay_noback style in overlay layout
    eyegui::setValueOfStyleAttribute(
        _pOverlayLayout,
        "tab_overlay_noback",
        "color",
        RGBAToHexString(colorAccent));
    eyegui::setValueOfStyleAttribute(
        _pOverlayLayout,
        "tab_overlay_noback",
        "color",
        RGBAToHexString(colorAccent));

    // Set color of tab_overlay_panel style in overlay layout
    eyegui::setValueOfStyleAttribute(
        _pOverlayLayout,
        "tab_overlay_panel",
        "color",
        RGBAToHexString(backgroundAccentColor));
    eyegui::setValueOfStyleAttribute(
        _pOverlayLayout,
        "tab_overlay_panel",
        "background-color",
        RGBAToHexString(panelBackgroundAccentColor));

    // Set color of tab_overlay style in scrolling overlay layout
    eyegui::setValueOfStyleAttribute(
        _pScrollingOverlayLayout,
        "tab_overlay",
        "color",
        RGBAToHexString(colorAccent));
}

void Tab::ActivateMode(TabMode mode)
{
    switch(mode)
    {
    case TabMode::READ:
        ActivateReadMode();
        break;
    case TabMode::INTERACTION:
        ActivateInteractionMode();
        break;
    case TabMode::CURSOR:
        ActivateCursorMode();
        break;
    }
}

void Tab::DeactivateMode(TabMode mode)
{
    switch(mode)
    {
    case TabMode::READ:
        DeactivateReadMode();
        break;
    case TabMode::INTERACTION:
        DeactivateInteractionMode();
        break;
    case TabMode::CURSOR:
        DeactivateCursorMode();
        break;
    }
}

void Tab::ActivateReadMode()
{
    // TODO
}

void Tab::DeactivateReadMode()
{
    // TODO
}

void Tab::ActivateInteractionMode()
{
    // TODO
}

void Tab::DeactivateInteractionMode()
{
    // TODO
}

void Tab::ActivateCursorMode()
{
    // TODO
}

void Tab::DeactivateCursorMode()
{
    // TODO
}

void Tab::DrawDebuggingOverlay() const
{
	// Bind debug render item
	_upDebugRenderItem->Bind();

	// Projection
	glm::mat4 projection = glm::ortho(0, 1, 0, 1);

	// Calculate model matrix
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(1.f / _pMaster->GetWindowWidth(), 1.f / _pMaster->GetWindowHeight(), 1.f));
	model = glm::translate(model, glm::vec3(100, 100, 1));
	model = glm::scale(model, glm::vec3(100, 100, 0));

	// Render all triggers etc. TODO

	// Combine matrics
	glm::mat4 matrix = projection * model;

	// Fill uniforms
	_upDebugRenderItem->GetShader()->UpdateValue("matrix", matrix);
	_upDebugRenderItem->GetShader()->UpdateValue("color", glm::vec3(0,1,1));
	
	// Render rectangle
	_upDebugRenderItem->Draw(GL_LINE_STRIP);
}

void Tab::TabButtonListener::down(eyegui::Layout* pLayout, std::string id)
{
    if(pLayout == _pTab->_pPanelLayout)
    {
        // ### Tab layout ###
        if(id == "click_mode")
        {
            _pTab->PushBackPipeline(std::move(std::unique_ptr<ZoomClickPipeline>(new ZoomClickPipeline(_pTab))));
        }
        else if(id == "auto_scrolling")
        {
            _pTab->_autoScrolling = true;
        }
        else if (id == "scroll_to_top")
        {
            _pTab->_pCefMediator->ResetScrolling(_pTab);
        }
        else if (id == "zoom")
        {
            _pTab->_zoomLevel = 1.3;

            // Trigger zooming in CefMediator
            _pTab->_pCefMediator->SetZoomLevel(_pTab);
        }
    }
    else
    {
        // ### Pipeline abort layout ###
        if(id == "abort")
        {
            _pTab->AbortAndClearPipelines();
        }
    }
}

void Tab::TabButtonListener::up(eyegui::Layout* pLayout, std::string id)
{
    if(pLayout == _pTab->_pPanelLayout)
    {
        // ### Tab layout ###
        if(id == "auto_scrolling")
        {
            _pTab->_autoScrolling = false;
        }
        else if (id == "zoom")
        {
            _pTab->_zoomLevel = 1.0;

            // Trigger zooming in CefMediator
            _pTab->_pCefMediator->SetZoomLevel(_pTab);
        }
    }
    else
    {
        // ### Pipeline abort layout ###
    }}

void Tab::TabSensorListener::penetrated(eyegui::Layout* pLayout, std::string id, float amount)
{
    if(pLayout == _pTab->_pScrollingOverlayLayout)
    {
        if(id == "scroll_up")
        {
            _pTab->_pCefMediator->EmulateMouseWheelScrolling(_pTab, 0, amount * 20.f);
        }
        else if(id == "scroll_down")
        {
            _pTab->_pCefMediator->EmulateMouseWheelScrolling(_pTab, 0, amount * -20.f);
        }
    }
}

void Tab::TabOverlayButtonListener::down(eyegui::Layout* pLayout, std::string id)
{
    // Search for id in map
    auto iter = _pTab->_overlayButtonDownCallbacks.find(id);
    if (iter != _pTab->_overlayButtonDownCallbacks.end())
    {
        // Execute callback
        iter->second();
    }
}

void Tab::TabOverlayButtonListener::up(eyegui::Layout* pLayout, std::string id)
{
    // Search for id in map
    auto iter = _pTab->_overlayButtonUpCallbacks.find(id);
    if (iter != _pTab->_overlayButtonUpCallbacks.end())
    {
        // Execute callback
        iter->second();
    }
}

void Tab::TabOverlayKeyboardListener::keyPressed(eyegui::Layout* pLayout, std::string id, std::u16string value)
{
    // Search for id in map
    auto iter = _pTab->_overlayKeyboardCallbacks.find(id);
    if (iter != _pTab->_overlayKeyboardCallbacks.end())
    {
        // Execute callback
        iter->second(value);
    }
}

void Tab::TabOverlayWordSuggestListener::chosen(eyegui::Layout* pLayout, std::string id, std::u16string value)
{
    // Search for id in map
    auto iter = _pTab->_overlayWordSuggestCallbacks.find(id);
    if (iter != _pTab->_overlayWordSuggestCallbacks.end())
    {
        // Execute callback
        iter->second(value);
    }
}
