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
    if(!tabInput.gazeUsed) // TODO: gazeUsed really good idea here? Maybe later null pointer?
    {
        if (tabInput.insideWebView)
        {
			// Calculate current raw position
			glm::vec2 newCoordinate(tabInput.webViewGazeRelativeX, tabInput.webViewGazeRelativeY);
			newCoordinate += _coordinateCenterOffset;

            // Speed of zooming
            float zoomSpeed;

			// Update deviation value (fade away deviation)
			_deviation = glm::max(0.f, _deviation - (tpf * 0.25f));

            // Update coordinate
            if (!_firstUpdate)
            {
                // Delta of new position
                glm::vec2 delta = newCoordinate - _coordinate;

				// Add length of delta to deviation
				// TODO: X and Y are not equally scaled, since relative values are used...
				_deviation += glm::length(delta) * tpf;

                // The bigger the distance, the slower the zoom
                zoomSpeed = 0.5 * (1.f - glm::length(delta));

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
            _linZoom -= 0.5f * tpf;
            _linZoom = glm::max(1.f, _linZoom);
        }

        // Click position to center offset
        float clickPositionCenterOffsetWeight = 0.5f;
        _coordinateCenterOffset = clickPositionCenterOffsetWeight * (1.0f - _logZoom) * (_coordinate - 0.5f);

        // Make zoom better with log function (and remember it for coordiante interpolation in next iteration)
        _logZoom = 1.0f - std::log(_linZoom);

        // Tell web view about zoom
        WebViewParameters webViewParameters;
        webViewParameters.centerOffset = _coordinateCenterOffset;
        webViewParameters.zoom = _logZoom;
        webViewParameters.zoomPosition = _coordinate;
        _pTab->SetWebViewParameters(webViewParameters);

        // Check, whether click is done
		if (
			_logZoom <= 0.075f // just zoomed so far into that coordinate is used
			|| (_logZoom <= 0.5f && _deviation < 0.02f)) // coordinate seems to be quite fixed, just do it
        {
            SetOutputValue("coordinate", _coordinate);
            return true;
        }
    }

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
