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
class DOMNode;

class TabActionInterface
{
public:

    // Push back a pipeline
    virtual void PushBackPipeline(std::unique_ptr<Pipeline> upPipeline) = 0;

    // Emulate click in tab. Optionally converts screen pixel position to rendered pixel position before calling CEF method
    virtual void EmulateLeftMouseButtonClick(double x, double y, bool visualize = true, bool isScreenCoordinate = true) = 0;

	// Emulate mouse cursor in tab. Optionally converts screen pixel position to rendered pixel position before calling CEF method
	virtual void EmulateMouseCursor(double x, double y, bool isScreenCoordinate = true) = 0;

    // Emulate mouse wheel scrolling
    virtual void EmulateMouseWheelScrolling(double deltaX, double deltaY) = 0;

    // Set text in text input field
    virtual void InputTextData(int64 frameID, int nodeID, std::string text, bool submit) = 0;

    // Get distance to next link and weak pointer to it. Returns empty weak pointer if no link available. Distance in screen coordinates
    virtual std::weak_ptr<const DOMNode> GetNearestLink(glm::vec2 pageCoordinate, float& rDistance) const = 0;

    // ### METHODS WHICH SET PARAMETERS THAT MUST BE RESET WHEN NO PIPELINE / ACTION IS ACTIVE ###

    // Reset method (called by pipeline at destruction, finish and abort)
    void Reset()
    {
        SetWebViewParameters(WebViewParameters());
    }

    // Set WebViewParameters for web view
    virtual void SetWebViewParameters(WebViewParameters parameters) = 0;

protected:

};

#endif // TABACTIONINTERFACE_H_
