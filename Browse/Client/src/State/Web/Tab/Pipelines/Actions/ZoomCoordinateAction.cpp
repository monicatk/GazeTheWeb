//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "ZoomCoordinateAction.h"
#include "src/State/Web/Tab/TabInteractionInterface.h"

ZoomCoordinateAction::ZoomCoordinateAction(TabInteractionInterface* pTab) : Action(pTab)
{
    // Add in- and output data slots
    AddVec2OutputSlot("coordinate");

    // Initialize members
    _coordinateCenterOffset = glm::vec2(0, 0);
}

bool ZoomCoordinateAction::Update(float tpf, TabInput tabInput)
{
    if(!tabInput.gazeUsed && tabInput.insideWebView) // TODO: gazeUsed really good idea here? Maybe later null pointer?
    {
		// Calculate current raw position (TODO: x and y are not equally scaled, because webview is not rectangular)
		glm::vec2 newCoordinate(tabInput.webViewGazeRelativeX, tabInput.webViewGazeRelativeY);
		newCoordinate += _coordinateCenterOffset;

		// Aspect ration correction (assuming wider than higher web view)
		int webViewWidth = 0;
		int webViewHeight = 0;
		_pTab->GetWebViewTextureResolution(webViewWidth, webViewHeight);
		float aspectRatioCorrection = 1.f;
		if (webViewWidth != 0 && webViewHeight != 0)
		{
			aspectRatioCorrection = (float)webViewWidth / (float)webViewHeight;
		}

        // Speed of zooming
        float zoomSpeed;

		// Update deviation value (fade away deviation)
		_deviation = glm::clamp(_deviation - (tpf * 0.125f), 0.f, 1.f);

        // Update coordinate
        if (!_firstUpdate)
        {
            // Delta of new relative position
            glm::vec2 delta = newCoordinate - _coordinate;

			// Do aspect correction for delta
			delta.x = delta.x / aspectRatioCorrection;

			// Add length of delta to deviation
			_deviation += glm::length(delta) * tpf;

            // The bigger the distance, the slower the zoom
            zoomSpeed = 0.6f * (1.f - glm::min(1.f, glm::length(delta))); // [0, 0.75]

			// If at the moment a high deviation is given, try to zoom out to give user more overview
			zoomSpeed = zoomSpeed - glm::min(1.f, 50.f * _deviation); // [-0.25, 0.75]

            // Move to new click position (weighted by zoom level for more smoohtness at higher zoom)
            float coordinateInterpolationSpeed = 5.f;
            _coordinate = _coordinate + delta * glm::min(1.0f, _logZoom * coordinateInterpolationSpeed * tpf);
        }
        else
        {
            // First frame of new click
            _coordinate = newCoordinate;
            zoomSpeed = 0.0f;
            _firstUpdate = false;
        }

        // Calculate zoom
        _linZoom += tpf * zoomSpeed;
	}
	else
	{
		// Zoom out when gaze not upon web view
		_linZoom -= 0.5f * tpf;
	}

	// Clamp linear zoom (one is standard, everything higher is zoomed)
	_linZoom = glm::max(_linZoom, 1.f);

	// Click position to center offset
	_coordinateCenterOffset = _clickPositionCenterOffsetWeight * (1.0f - _logZoom) * (_coordinate - 0.5f);

	// Make zoom better with log function (and remember it for coordiante interpolation in next iteration)
	_logZoom = 1.0f - std::log(_linZoom);

	// Check, whether click is done
	if (
		_logZoom <= 0.075f // just zoomed so far into that coordinate is used
		|| (_logZoom <= 0.25f && _deviation < 0.02f)) // coordinate seems to be quite fixed, just do it
	{
		SetOutputValue("coordinate", _coordinate);
		return true;
	}

	// Decrement dimming
	_dimming += tpf;
	_dimming = glm::min(_dimming, _dimmingDuration);

	// Tell web view about zoom
	WebViewParameters webViewParameters;
	webViewParameters.centerOffset = _coordinateCenterOffset;
	webViewParameters.zoom = _logZoom;
	webViewParameters.zoomPosition = _coordinate;
	webViewParameters.dim = _dimmingValue * (_dimming / _dimmingDuration);
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
