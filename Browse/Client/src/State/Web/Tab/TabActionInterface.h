//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Tab interface for manipulation from actions.

#ifndef TABACTIONINTERFACE_H_
#define TABACTIONINTERFACE_H_

#include "src/State/Web/Tab/WebViewParameters.h"
#include "src/Typedefs.h"
#include <memory>
#include <string>

// Forward declaration
class Pipeline;

// Modes
enum class TabMode { READ, INTERACTION, CURSOR };

class TabActionInterface
{
public:

    //  Push back a pipeline
    virtual void PushBackPipeline(std::unique_ptr<Pipeline> upPipeline) = 0;

    // Emulate click in tab
    virtual void EmulateLeftMouseButtonClick(double x, double y) = 0;

    // Emulate mouse wheel scrolling
    virtual void EmulateMouseWheelScrolling(double deltaX, double deltaY) = 0;

    // Set text in text input field
    virtual void InputTextData(int64 frameID, int nodeID, std::string text, bool submit) = 0;

    // Get current web view resolution. Sets to 0 if not possible
    virtual void GetWebViewTextureResolution(int& rWidth, int& rHeight) const = 0;

    // Set next mode (will be set after pipelines finish)
    virtual void SetNextMode(TabMode mode) { _nextMode = mode; }

    // ### METHODS WHICH SET PARAMETERS THAT MUST BE RESET WHEN NO PIPELINE / ACTION IS ACTIVE ###

    // Reset method (called by pipeline at destruction, finish and abort)
    void Reset()
    {
        SetWebViewParameters(WebViewParameters());
    }

    // Set WebViewParameters for web view
    virtual void SetWebViewParameters(WebViewParameters parameters) = 0;

protected:

    // Next mode
    TabMode _nextMode = TabMode::READ;
};

#endif // TABACTIONINTERFACE_H_
