//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "MagnificationCoordinateAction.h"
#include "src/State/Web/Tab/Interface/TabInteractionInterface.h"
#include "submodules/glm/glm/gtx/vector_angle.hpp"
#include <algorithm>

MagnificationCoordinateAction::MagnificationCoordinateAction(TabInteractionInterface* pTab, bool doDimming) : Action(pTab)
{
	// Save members
	_doDimming = doDimming;

    // Add in- and output data slots
    AddVec2OutputSlot("coordinate");
}

bool MagnificationCoordinateAction::Update(float tpf, TabInput tabInput)
{
	// Speed of zooming
	float zoomSpeed = 0.f;

	// Function transforms coordinate from relative WebView coordinates to CEFPixel coordinates on page
	const std::function<void(const float&, const glm::vec2&, const glm::vec2&, glm::vec2&)> pageCoordinate
		= [&](const float& rLogZoom, const glm::vec2& rRelativeZoomCoordinate, const glm::vec2& rRelativeCenterOffset, glm::vec2& rCoordinate)
	{
		// Analogous to shader in WebView
		rCoordinate += rRelativeCenterOffset; // add center offset
		rCoordinate -= rRelativeZoomCoordinate; // move zoom coordinate to origin
		rCoordinate *= rLogZoom; // apply zoom
		rCoordinate += rRelativeZoomCoordinate; // move back
		rCoordinate *= glm::vec2(_pTab->GetWebViewResolutionX(), _pTab->GetWebViewResolutionY()); // bring into pixel space of CEF
	};

	// Function calling above function with current values
	const std::function<void(glm::vec2&)> currentPageCoordinate
		= [&](glm::vec2& rCoordinate)
	{
		pageCoordinate(_logZoom, _relativeZoomCoordinate, _relativeCenterOffset, rCoordinate);
	};

	// Current gaze
	glm::vec2 relativeGazeCoordinate = glm::vec2(tabInput.webViewGazeRelativeX, tabInput.webViewGazeRelativeY); // relative WebView space

	// Only allow zoom in when gaze upon web view
	if (!tabInput.gazeUsed && tabInput.insideWebView) // TODO: gazeUsed really good idea here? Maybe later null pointer?
	{
		
	}

	// Update linear zoom
	_linZoom += tpf * zoomSpeed; // frame rate depended, because zoomSpeed is depending on deviation which is depending on tpf

	// Clamp linear zoom (one is standard, everything higher is zoomed)
	_linZoom = glm::max(_linZoom, 1.f);

	// Make zoom better with log function
	_logZoom = 1.f - std::log(_linZoom); // log zooming is starting at one and getting smaller with smaller _linZoom

	// Calculate coordinate for current gaze value
	glm::vec2 pixelGazeCoordinate = relativeGazeCoordinate;
	currentPageCoordinate(pixelGazeCoordinate);

	// Decide whether zooming is finished
	bool finished = false;
	if (!tabInput.gazeUsed && tabInput.instantInteraction) // user demands on instant interaction
	{
		// Set coordinate in output value. Use current gaze position
		

		// Return success
		finished = true;
	}
	else if ( // zooming is high enough for coordinate to be accurate
		_logZoom <= MAX_LOG_ZOOM // just zoomed so far into that coordinate is just used
		|| ((_logZoom <= 0.45f) && (_deviation < 0.01f))) // coordinate seems to be quite fixed, just do it
	{
		// Set coordinate in output value
        SetOutputValue("coordinate", glm::vec2(_relativeZoomCoordinate * glm::vec2(_pTab->GetWebViewResolutionX(), _pTab->GetWebViewResolutionY()))); // into pixel space of CEF

		// Return success
		finished = true;
	}

	SetOutputValue("coordinate", pixelGazeCoordinate);

	// Decrement dimming
	_dimming += tpf;
	_dimming = glm::min(_dimming, DIMMING_DURATION);

	// Tell web view about zoom
	WebViewParameters webViewParameters;
	webViewParameters.centerOffset = _relativeCenterOffset;
	webViewParameters.zoom = _logZoom;
	webViewParameters.zoomPosition = _relativeZoomCoordinate;
	if (_doDimming) { webViewParameters.dim = DIMMING_VALUE * (_dimming / DIMMING_DURATION); }
	_pTab->SetWebViewParameters(webViewParameters);

    // Return whether finished
    return finished;
}

void MagnificationCoordinateAction::Draw() const
{

}

void MagnificationCoordinateAction::Activate()
{

}

void MagnificationCoordinateAction::Deactivate()
{
	// Reset web view (necessary because of dimming)
	WebViewParameters webViewParameters;
	_pTab->SetWebViewParameters(webViewParameters);
}

void MagnificationCoordinateAction::Abort()
{

}
