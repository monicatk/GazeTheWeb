//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "ZoomCoordinateAction.h"
#include "src/State/Web/Tab/Interface/TabInteractionInterface.h"
#include "submodules/glm/glm/gtx/vector_angle.hpp"
#include <algorithm>

ZoomCoordinateAction::ZoomCoordinateAction(TabInteractionInterface* pTab, bool doDimming) : Action(pTab)
{
	// Save members
	_doDimming = doDimming;

    // Add in- and output data slots
    AddVec2OutputSlot("coordinate");
}

bool ZoomCoordinateAction::Update(float tpf, TabInput tabInput)
{
	// Update zoom data queue by incrementing life time and removing data sets which are too old
	std::for_each(_zoomDataQueue.begin(), _zoomDataQueue.end(), [&](ZoomData& rZoomData) { rZoomData.lifetime += tpf; });
	_zoomDataQueue.erase(
		std::remove_if(
			_zoomDataQueue.begin(),
			_zoomDataQueue.end(),
			[&](const ZoomData& rZoomData) { return rZoomData.lifetime > LOOK_BACK_TIME; }),
		_zoomDataQueue.end());

	// Speed of zooming
	float zoomSpeed = 0.f;

	// Pixels in web view
	int webViewWidth = _pTab->GetWebViewWidth();
	int webViewHeight = _pTab->GetWebViewHeight();

	// Only allow zoom in when gaze upon web view
    if(!tabInput.gazeUsed && tabInput.insideWebView) // TODO: gazeUsed really good idea here? Maybe later null pointer?
    {
		// Calculate current raw coordinate in web view relative coordinates. The filtered coordinate will move towards that position
		glm::vec2 newCoordinate(tabInput.webViewGazeRelativeX, tabInput.webViewGazeRelativeY);
		newCoordinate += _coordinateCenterOffset;

		// Update deviation value (fade away deviation)
		_deviation = glm::max(_deviation - (tpf / DEVIATION_FADING_DURATION), 0.f);

        // Update coordinate
        if (!_firstUpdate)
        {
            // Delta of new relative position
            glm::vec2 rawDelta = newCoordinate - _coordinate;

			// Move to new click position
			_coordinate += rawDelta * (tpf / MOVE_DURATION);

			// Aspect ratio correction
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

			// Do aspect correction for delta
			glm::vec2 delta = rawDelta / aspectRatioCorrection;

			// Set length of delta to deviation if bigger than current deviation
			_deviation = glm::min(1.f, glm::max(glm::length(delta), _deviation));			

			// If at the moment a high deviation is given, try to zoom out to give user more overview
			zoomSpeed = ZOOM_SPEED - glm::min(1.f, 3.f * _deviation); // [-0.5, 0.5]
        }
        else // first frame of execution
        {
			// Use raw coordinate as new coordinate
            _coordinate = newCoordinate;

			// Since only for first frame, do not do it again
            _firstUpdate = false;
        }
	}

	// Update linear zoom
	_linZoom += tpf * zoomSpeed; // frame rate depended, because zoomSpeed is depending on deviation which is depending on tpf

	// Clamp linear zoom (one is standard, everything higher is zoomed)
	_linZoom = glm::max(_linZoom, 1.f);

	// Make zoom better with log function
	_logZoom = 1.f - std::log(_linZoom); // log zooming is starting at one and getting smaller with smaller _linZoom

	// Calculated center offset
	_coordinateCenterOffset =
		CENTER_OFFSET_MULTIPLIER
		* (1.f - _logZoom) // weight with zoom (starting at zero) to have more centered version at higher zoom level
		* (_coordinate - 0.5f); // vector from current zoom coordinate to center of web view center

    // Vector with web view pixel resolution
    glm::vec2 webViewPixels(webViewWidth, webViewHeight);

	// Calculate coordinate for current gaze value
	glm::vec2 pixelGazeCoordinate =
		glm::vec2(
			tabInput.webViewGazeRelativeX,
			tabInput.webViewGazeRelativeY)
		+ _coordinateCenterOffset; // add center offset
	pixelGazeCoordinate -= _coordinate; // move zoom position to origin
	pixelGazeCoordinate *= _logZoom; // apply zoom
	pixelGazeCoordinate += _coordinate; // move back
	pixelGazeCoordinate *= webViewPixels; // into pixel space

	// Check, whether click is done
	if (
		_logZoom <= 0.075f // just zoomed so far into that coordinate is used
		|| ((_logZoom <= 0.5f) && (_deviation < 0.0125f))) // coordinate seems to be quite fixed, just do it
	{
		int zoomDataIndex = -1;

		// Find zoom data where user starts to calm down gaze
		int i = 0;
		if (!_zoomDataQueue.empty())
		{
			for (const auto& rZoomData : _zoomDataQueue)
			{
				// Compare gaze drift with zoom coordinate drift
				glm::vec2 pixelGazeDrift = pixelGazeCoordinate - rZoomData.pixelGazeCoordinate;
				glm::vec2 pixelCoordinateDrift = (_coordinate * webViewPixels) - rZoomData.pixelZoomCoordinate;

				// Test angle between vectors
				if (glm::degrees(glm::angle(glm::normalize(pixelGazeDrift), glm::normalize(pixelCoordinateDrift))) <= DRIFT_MAX_ANGLE_DEGREE)
				{
					zoomDataIndex = i;
					break;
				}

				// Increment index counter
				i++;
			}
		}

		// Try some calibration error compensation if available data is good enough
        /*if (zoomDataIndex >= 0)
		{
			// Get valid zoom data set with good quality
			ZoomData old = _zoomDataQueue.at(zoomDataIndex);

			// Calculate drift of gaze
			glm::vec2 pixelGazeDrift = pixelGazeCoordinate - old.pixelGazeCoordinate; // drift of gaze coordinates
            float pixelGazeDriftLength = glm::length(pixelGazeDrift); // length of drift in pixels

            // Calculate drift of zoom coordinates
            glm::vec2 pixelCoordinateDrift = (_coordinate * webViewPixels) - old.pixelZoomCoordinate;

            // Subtract distance between zoom coordinates
            pixelGazeDriftLength -= glm::length(pixelCoordinateDrift);

            // Radius around old zoom coordinate where actual fixation point should be
            float pixelRadius = pixelGazeDriftLength / (old.logZoom - _logZoom);

            // Calculate fixation coordinates
            glm::vec2 pixelFixationCoordinate = (glm::normalize(pixelCoordinateDrift) * pixelRadius) + old.pixelZoomCoordinate;

			// Set coordinate in output value 
            SetOutputValue("coordinate", pixelFixationCoordinate);
		}
        else*/
		{
			// Set coordinate in output value 
            SetOutputValue("coordinate", glm::vec2(_coordinate * webViewPixels));
		}

        // Reset web view
        WebViewParameters webViewParameters;
        _pTab->SetWebViewParameters(webViewParameters);

		// Return success
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
	if (_doDimming) { webViewParameters.dim = DIMMING_VALUE * (_dimming / DIMMING_DURATION); }
	_pTab->SetWebViewParameters(webViewParameters);

	// Save values in queue
	ZoomData zoomData;
	zoomData.pixelGazeCoordinate = pixelGazeCoordinate; // use already calculated one
	zoomData.pixelZoomCoordinate = _coordinate * webViewPixels; // into pixel space
	zoomData.logZoom = _logZoom;
	_zoomDataQueue.push_back(zoomData);

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
