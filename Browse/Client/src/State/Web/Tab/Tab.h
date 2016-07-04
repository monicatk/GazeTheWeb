//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Müller (muellerd@uni-koblenz.de)
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Class for tabs. Registers itself in CefMediator. Implements all Tab
// interfaces. Has queue of pipelines which are executed in the order they are
// pushed back.

#ifndef TAB_H_
#define TAB_H_

#include "src/State/Web/Tab/TabInteractionInterface.h"
#include "src/State/Web/Tab/TabCEFInterface.h"
#include "src/State/Web/Tab/DOMNode.h"
#include "src/State/Web/Tab/WebView.h"
#include "src/State/Web/Tab/Pipelines/Pipeline.h"
#include "src/State/Web/Tab/Triggers/DOMTrigger.h"
#include "src/Utils/Input.h"
#include "src/Global.h"
#include "submodules/glm/glm/glm.hpp"
#include "submodules/eyeGUI/include/eyeGUI.h"
#include <vector>
#include <deque>
#include <memory>
#include <string>
#include <map>

// Forward declaration
class Master;
class CefMediator;

// Class
class Tab : public TabInteractionInterface, public TabCEFInterface
{
public:

    // Constructor
    Tab(Master* pMaster, CefMediator* pCefMediator, std::string url);

    // Destructor
    virtual ~Tab();

    // Update
    void Update(float tpf, Input& rInput);

    // Draw
    void Draw() const;

    // Activate
    void Activate();

    // Deactivate
    void Deactivate();

    // Open URL. Does load it
    void OpenURL(std::string URL);

    // Abort pipeline
    void AbortAndClearPipelines();

    // Open next page
    void GoForward();

    // Open previous page
    void GoBack();

    // Reload page
    void Reload();

    // Can go back / forward
    bool CanGoBack() const { return _canGoBack; }
    bool CanGoForward() const { return _canGoForward; }

    // #################################
    // ### TAB INTERACTIVE INTERFACE ###
    // #################################

    // Calculate position and size of web view
    virtual void CalculateWebViewPositionAndSize(int& rX, int& rY, int& rWidth, int& rHeight) const;

    // Getter for window with and height
    virtual void GetWindowSize(int& rWidth, int& rHeight) const;

    // #############################
    // ### TAB OVERLAY INTERFACE ###
    // #############################

    // Add floating frame from overlay (not visible after creation)
    virtual int AddFloatingFrameToOverlay(
        std::string brickFilepath,
        float relativePositionX,
        float relativePositionY,
        float relativeSizeX,
        float relativeSizeY,
        std::map<std::string, std::string> idMapper);

    // Move floating frame in overlay
    virtual void SetPositionOfFloatingFrameInOverlay(
        int index,
        float relativePositionX,
        float relativePositionY);

    // Set visibility of floating frame in overlay
    virtual void SetVisibilyOfFloatingFrameInOverlay(int index, bool visible);

    // Remove floating frame from overlay
    virtual void RemoveFloatingFrameFromOverlay(int index);

    // Register button listener in overlay
    virtual void RegisterButtonListenerInOverlay(std::string id, std::function<void(void)> downCallback, std::function<void(void)> upCallback);

    // Unregister button listener callback in overlay
    virtual void UnregisterButtonListenerInOverlay(std::string id);

    // Register keyboard listener in overlay
    virtual void RegisterKeyboardListenerInOverlay(std::string id, std::function<void(std::u16string)> callback);

    // Unregister keyboard listener callback in overlay
    virtual void UnregisterKeyboardListenerInOverlay(std::string id);

    // Set case of keyboard letters
    virtual void SetCaseOfKeyboardLetters(std::string id, bool upper);

    // Register word suggest listener in overlay
    virtual void RegisterWordSuggestListenerInOverlay(std::string id, std::function<void(std::u16string)> callback);

    // Unregister word suggest listener callback in overlay
    virtual void UnregisterWordSuggestListenerInOverlay(std::string id);

    // Use word suggest to display suggestions. Chosen one can be received by callback. Empty input clears suggestions
    virtual void DisplaySuggestionsInWordSuggest(std::string id, std::u16string input);

    // Get scrolling offset
    virtual void GetScrollingOffset(double& rScrollingOffsetX, double& rScrollingOffsetY) const;

    // Set content of text block
    virtual void SetContentOfTextBlock(std::string id, std::u16string content);

    // ############################
    // ### TAB ACTION INTERFACE ###
    // ############################

    // Push back an pipeline
    virtual void PushBackPipeline(std::unique_ptr<Pipeline> upPipeline);

    // Emulate left mouse button click
    virtual void EmulateLeftMouseButtonClick(double x, double y);

    // Emulate mouse wheel scrolling
    virtual void EmulateMouseWheelScrolling(double deltaX, double deltaY);

    // Set text in text input field
    virtual void InputTextData(int64 frameID, int nodeID, std::string text, bool submit);

    // Get current web view resolution. Sets to 0 if not possible
    virtual void GetWebViewTextureResolution(int& rWidth, int& rHeight) const;

    // Set WebViewParameters for WebView
    virtual void SetWebViewParameters(WebViewParameters parameters) { _webViewParameters = parameters; }

    // #########################
    // ### TAB CEF INTERFACE ###
    // #########################

    // Tell CEF callback which resolution web view texture should have
    virtual void GetWebRenderResolution(int& rWidth, int& rHeight) const;

    // Getter and setter for favicon URL
    virtual std::string GetFavIconURL() const { return _favIconUrl; }
    virtual void SetFavIconURL(std::string url) { _favIconUrl = url; }

    // Setter of URL. Does not load it. Should be called by CefMediator only
    virtual void SetURL(std::string URL);

    // Setter for can go back / go forward
    virtual void SetCanGoBack(bool canGoBack) { _canGoBack = canGoBack;	}
    virtual void SetCanGoForward(bool canGoForward) { _canGoForward = canGoForward; }

    // Receive favicon bytes as char vector ordered in RGBA. Accepts also NULL for upData!
    virtual void ReceiveFaviconBytes(std::unique_ptr< std::vector<unsigned char> > upData, int width, int height);
	virtual void ResetFaviconBytes(); // TODO

    // Get weak pointer to texture of web view
    virtual std::weak_ptr<Texture> GetWebViewTexture();

    // Used by DOMMapping interface
    virtual void AddDOMNode(std::shared_ptr<DOMNode> spNode);
    virtual void ClearDOMNodes();

    // Receive callbacks from CefMediator upon scrolling offset changes
    virtual void SetScrollingOffset(double x, double y);

    // Getter for URL
    virtual std::string GetURL() const { return _url; }

    // Getter for current zoom level of corresponding browser
    virtual double GetZoomLevel() const { return _zoomLevel; }

    // Set page resolution from Cef Mediator
    virtual void SetPageResolution(double width, double height);

    void SetFixedElementsCoordinates(std::vector<glm::vec4> elements);
    bool GetFixedElementsLoadedAfterScrolling();

private:

    // Give listener full access
    friend class TabButtonListener;
    friend class TabSensorListener;
    friend class TabOverlayButtonListener;

    // Listener for GUI
    class TabButtonListener: public eyegui::ButtonListener
    {
    public:

        TabButtonListener(Tab* pTab) { _pTab = pTab; }
        virtual void hit(eyegui::Layout* pLayout, std::string id) {}
        virtual void down(eyegui::Layout* pLayout, std::string id);
        virtual void up(eyegui::Layout* pLayout, std::string id);

    private:

        Tab* _pTab;
    };

    class TabSensorListener: public eyegui::SensorListener
    {
    public:

        TabSensorListener(Tab* pTab) { _pTab = pTab; }
        virtual void penetrated(eyegui::Layout* pLayout, std::string id, float amount);

    private:

        Tab* _pTab;
    };

    class TabOverlayButtonListener : public eyegui::ButtonListener
    {
    public:

        TabOverlayButtonListener(Tab* pTab) { _pTab = pTab; }
        virtual void hit(eyegui::Layout* pLayout, std::string id) {}
        virtual void down(eyegui::Layout* pLayout, std::string id);
        virtual void up(eyegui::Layout* pLayout, std::string id);

    private:

        Tab* _pTab;
    };

    class TabOverlayKeyboardListener : public eyegui::KeyboardListener
    {
    public:

        TabOverlayKeyboardListener(Tab* pTab) { _pTab = pTab; }
        virtual void keyPressed(eyegui::Layout* pLayout, std::string id, std::u16string value);
        virtual void keyPressed(eyegui::Layout* pLayout, std::string id, std::string value) {}

    private:

        Tab* _pTab;
    };

    class TabOverlayWordSuggestListener : public eyegui::WordSuggestListener
    {
    public:

        TabOverlayWordSuggestListener(Tab* pTab) { _pTab = pTab; }
        virtual void chosen(eyegui::Layout* pLayout, std::string id, std::u16string value);
        virtual void chosen(eyegui::Layout* pLayout, std::string id, std::string value) {}

    private:

        Tab* _pTab;
    };

    // Instance of listener
    std::shared_ptr<TabButtonListener> _spTabButtonListener;
    std::shared_ptr<TabSensorListener> _spTabSensorListener;
    std::shared_ptr<TabOverlayButtonListener> _spTabOverlayButtonListener;
    std::shared_ptr<TabOverlayKeyboardListener> _spTabOverlayKeyboardListener;
    std::shared_ptr<TabOverlayWordSuggestListener> _spTabOverlayWordSuggestListener;

    // Get reserved space for WebView in eyeGUI layout
    eyegui::AbsolutePositionAndSize CalculateWebViewPositionAndSize() const;

    // Set pipeline activity (indicates whether there is some pipeline to work on)
    void SetPipelineActivity(bool active);

    // Method to update and pipe accent color to eyeGUI
    void UpdateAccentColor(float tpf);

    // Activate mode (called by update method, only)
    void ActivateMode(TabMode mode);

    // Deactivate mode (called by update method, only)
    void DeactivateMode(TabMode mode);

    // Activate / deactivate ReadMode
    void ActivateReadMode();
    void DeactivateReadMode();

    // Activate / deactivate InteractionMode
    void ActivateInteractionMode();
    void DeactivateInteractionMode();

    // Activate / deactivate CursorMode
    void ActivateCursorMode();
    void DeactivateCursorMode();

    // Current URL
    std::string _url = "";

    // Vector with DOMTriggers
    std::vector<std::unique_ptr<DOMTrigger> >_DOMTriggers;

    // Web view in which website is rendered and displayed
    std::unique_ptr<WebView> _upWebView;

    // Pointer to master.
    Master* _pMaster;

    // Pipelines which shall be executed
    std::deque<std::unique_ptr<Pipeline> > _pipelines;

    // Layouts
    eyegui::Layout* _pPanelLayout;
    eyegui::Layout* _pPipelineAbortLayout;
    eyegui::Layout* _pOverlayLayout;
    eyegui::Layout* _pScrollingOverlayLayout;

    // Parameters for WebView
    WebViewParameters _webViewParameters;

    // Mode
    TabMode _mode = _nextMode; // use default next mode from interface here as default

    // Layout color accent
    glm::vec4 _targetColorAccent = TAB_DEFAULT_COLOR_ACCENT;
    glm::vec4 _currentColorAccent = TAB_DEFAULT_COLOR_ACCENT;
    float _colorInterpolation = 1.f;

    // Bool to indicate whether some pipeline is active
    bool _pipelineActive = false;

    // Automatic scrolling
    bool _autoScrolling = false;
    float _autoScrollingValue = 0; // [-1..1]

    // Level of zooming
    double _zoomLevel = 1;

    // Pointer to mediator
    CefMediator* _pCefMediator;

    // Frame indices of scroll up and down overlays
    unsigned int _scrollUpFrameIndex = 0;
    unsigned int _scrollDownFrameIndex = 0;

    // Ids of elements in overlay (added / removed by triggers or actions)
    std::map<std::string, std::function<void(void)> > _overlayButtonDownCallbacks;
    std::map<std::string, std::function<void(void)> > _overlayButtonUpCallbacks;
    std::map<std::string, std::function<void(std::u16string)> > _overlayKeyboardCallbacks;
    std::map<std::string, std::function<void(std::u16string)> > _overlayWordSuggestCallbacks;

    // Time until Cef is asked for page resolution
    float _timeUntilGetPageResolution = TAB_GET_PAGE_RES_INTERVAL;

    // ################################
    // ### ONLY SET BY CEF MEDIATOR ###
    // ################################

    // Current page size
    double _pageWidth = 0, _pageHeight = 0;

    // Coordinates of current fixed elements on site, vec4 = (top, left, bottom, right)
    std::vector<glm::vec4> _fixedElements;
    bool _fixedElementsLoadedAfterScrolling = false;	// TODO: Must be set to false everytime a new page is loaded

    // URL of current favIcon
    std::string _favIconUrl;

    // Scroll offset
    double _scrollingOffsetX = 0;
    double _scrollingOffsetY = 0;

    // Can go back / forward
    bool _canGoBack = false;
    bool _canGoForward = false;

};

#endif // TAB_H_
