//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "ZoomCoordinateAction.h"
#include "src/State/Web/Tab/Interface/TabInteractionInterface.h"
#include <iostream>

ZoomCoordinateAction::ZoomCoordinateAction(TabInteractionInterface* pTab) : Action(pTab)
{
    // Add in- and output data slots
    AddVec2OutputSlot("coordinate");

    // Initialize members
    _coordinateCenterOffset = glm::vec2(0, 0);
}

bool ZoomCoordinateAction::Update(float tpf, TabInput tabInput)
{
	// Speed of zooming
	float zoomSpeed;

	// Only allow zoom in when gaz upon web view
    if(!tabInput.gazeUsed && tabInput.insideWebView) // TODO: gazeUsed really good idea here? Maybe later null pointer?
    {
		// Aspect ration correction
		int webViewWidth = 0;
		int webViewHeight = 0;
		_pTab->GetWebViewTextureResolution(webViewWidth, webViewHeight);
		glm::vec2 aspectRatioCorrection(1.f, 1.f);
		if (webViewWidth != 0 && webViewHeight != 0)
		{
			if (webViewWidth > webViewHeight)
			{
				aspectRatioCorrection.x = (float)webViewWidth / (float)webViewHeight;
			}
			else
			{
				aspectRatioCorrection.y = (float)webViewHeight / (float)webViewWidth;
			}
		}

		// Calculate current raw position in web view relative coordinates
		glm::vec2 newCoordinate(tabInput.webViewGazeRelativeX, tabInput.webViewGazeRelativeY);
		newCoordinate += _coordinateCenterOffset;

		// Update deviation value (fade away deviation)
		_deviation = glm::max(_deviation - (tpf / DEVIATION_FADING_DURATION), 0.f);

        // Update coordinate
        if (!_firstUpdate)
        {
            // Delta of new relative position
            glm::vec2 rawDelta = newCoordinate - _coordinate;

			// Do aspect correction for delta
			glm::vec2 delta = rawDelta / aspectRatioCorrection;

			// Set length of delta to deviation if bigger than current deviation
			_deviation = glm::max(glm::length(delta), _deviation);			

            // Move to new click position (weighted by zoom level for more smoothness at higher zoom, since zoom value gets smaller at higher zoom)
            _coordinate += delta * tpf * _logZoom;

			// If at the moment a high deviation is given, try to zoom out to give user more overview
			zoomSpeed = ZOOM_SPEED - glm::min(1.f, 3.f * _deviation); // [-0.5, 0.5]
        }
        else
        {
            // First frame of new click
            _coordinate = newCoordinate;
            zoomSpeed = 0.0f;
            _firstUpdate = false;
        }
	}
	else
	{
		// Zoom out when gaze not upon web view
		zoomSpeed = -0.5f * ZOOM_SPEED;
	}

	// Update linear zoom
	_linZoom += tpf * zoomSpeed;

	// Clamp linear zoom (one is standard, everything higher is zoomed)
	_linZoom = glm::max(_linZoom, 1.f);

	// Click position to center offset.
	_coordinateCenterOffset =
		CENTER_OFFSET_MULTIPLIER
		* (1.f - _logZoom) // weight with zoom (starting at zero) to have more centered version at higher zoom level
		* (_coordinate - 0.5f); // vector from current zoom coordinate to center of web view center

	// Make zoom better with log function
	_logZoom = 1.f - std::log(_linZoom); // log zooming is starting at one and getting smaller with smaller _linZoom

	// Check, whether click is done
	if (
		_logZoom <= 0.075f // just zoomed so far into that coordinate is used
		|| (_logZoom <= 0.3f && _deviation < 0.03f)) // coordinate seems to be quite fixed, just do it
	{
		SetOutputValue("coordinate", _coordinate);
		return true;
	}

	// Decrement dimming
	_dimming += tpf;
	_dimming = glm::min(_dimming, DIMMING_DURATION);

	// Tell web view about zoom
	WebViewParameters webViewParameters;
	webViewParameters.centerOffset = _coordinateCenterOffset;
	webViewParameters.zoom = _logZoom;
	webViewParameters.zoomPosition = _coordinate;
	webViewParameters.dim = DIMMING_VALUE * (_dimming / DIMMING_DURATION);
	_pTab->SetWebViewParameters(webViewParameters);

    // Not finished, yet
    return false;
}

void ZoomCoordinateAction::Draw() const
{

}

void ZoomCoordinateAction::Activate()
{

}

void ZoomCoordinateAction::Deactivate()
{

}

void ZoomCoordinateAction::Abort()
{

}
