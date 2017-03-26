//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "ZoomCoordinateAction.h"
#include "src/State/Web/Tab/Interface/TabInteractionInterface.h"
#include "submodules/glm/glm/gtx/vector_angle.hpp"
#include "submodules/glm/glm/gtx/component_wise.hpp"
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
	// TODO
	// - delta for zoom coordinate maybe on page coordinates, not with web view (makes more sense)
	// - play around with deviation and move duration

	// ### Preparation ###

	// Speed of zooming
	float zoomSpeed = 0.f;

	// Pixels in web view
	int webViewWidth = _pTab->GetWebViewWidth();
	int webViewHeight = _pTab->GetWebViewHeight();
	glm::vec2 webViewPixels(webViewWidth, webViewHeight);

	// Function caclulate position of relative coordinate on actual CEFPixelSpace. Takes relative coordinate
	const std::function<void(glm::vec2&)> pageCoordinate = [&](glm::vec2& rRelativeCoordinate)
	{
		rRelativeCoordinate += _coordinateCenterOffset; // add center offset
		rRelativeCoordinate -= _coordinate; // move zoom position to origin
		rRelativeCoordinate *= _logZoom; // apply zoom
		rRelativeCoordinate += _coordinate; // move back
		rRelativeCoordinate *= webViewPixels; // into pixel space
	};

	// ### Update zoom speed and center offset ###

	// Only allow zoom in when gaze upon web view
	if (!tabInput.gazeUsed && tabInput.insideWebView) // TODO: gazeUsed really good idea here? Maybe later null pointer?
	{
		// ### Improve zoom coordinate ###
		if (!_driftCorrection)
		{
			// Calculate current raw coordinate in web view relative coordinates. The filtered coordinate will move towards that position
			glm::vec2 newCoordinate(tabInput.webViewGazeRelativeX, tabInput.webViewGazeRelativeY);
			newCoordinate += _coordinateCenterOffset;

			// Update deviation value (fade away deviation)
			_deviation = glm::max(_deviation - (tpf / DEVIATION_FADING_DURATION), 0.f);

			// Update coordinate
			if (!_firstUpdate)
			{
				// Get coordinates on actual page
				glm::vec2 pixelNewCoordinate = newCoordinate;
				pageCoordinate(pixelNewCoordinate);
				glm::vec2 pixelCoordinate = _coordinate;
				pageCoordinate(pixelCoordinate);

				// Delta of coordinates
				glm::vec2 delta = (pixelNewCoordinate - pixelCoordinate) / glm::compMax(webViewPixels); // [0..1]

				// Move to new click position
				_coordinate += delta * (tpf / MOVE_DURATION);

				// Set length of delta to deviation if bigger than current deviation
				_deviation = glm::min(1.f, glm::max(glm::length(delta), _deviation));

				// If at the moment a high deviation is given, try to zoom out to give user more overview
				zoomSpeed = ZOOM_SPEED - glm::min(1.f, 3.f * _deviation); // TODO weight deviation more intelligent
			}
			else // first frame of execution
			{
				// Use raw coordinate as new coordinate
				_coordinate = newCoordinate;

				// Since only for first frame, do not do it again
				_firstUpdate = false;
			}

			// Calculated center offset
			_coordinateCenterOffset =
				CENTER_OFFSET_MULTIPLIER
				* (1.f - _logZoom) // weight with zoom (starting at zero) to have more centered version at higher zoom level
				* (_coordinate - 0.5f); // vector from current zoom coordinate to center of web view center
		}
		else
		{
			// ### Do just zooming to calculate gaze drift ###
			zoomSpeed = 0.25f;

			// TODO: make this more smooth (maybe even continue to center it further)
		}
	}

	// ### Update zoom ###

	// Update linear zoom
	_linZoom += tpf * zoomSpeed; // frame rate depended, because zoomSpeed is depending on deviation which is depending on tpf

	// Clamp linear zoom (one is standard, everything higher is zoomed)
	_linZoom = glm::max(_linZoom, 1.f);

	// Make zoom better with log function
	_logZoom = 1.f - glm::max(0.f, glm::log(_linZoom)); // log zooming is starting at one and getting smaller with higher _linZoom

	// ### Update values ###

	// Calculate coordinate for current gaze value
	glm::vec2 pixelGazeCoordinate = glm::vec2(tabInput.webViewGazeRelativeX, tabInput.webViewGazeRelativeY);
	pageCoordinate(pixelGazeCoordinate);

	/*
		glm::vec2(
			tabInput.webViewGazeRelativeX,
			tabInput.webViewGazeRelativeY)
		+ _coordinateCenterOffset; // add center offset
	pixelGazeCoordinate -= _coordinate; // move zoom position to origin
	pixelGazeCoordinate *= _logZoom; // apply zoom
	pixelGazeCoordinate += _coordinate; // move back
	pixelGazeCoordinate *= webViewPixels; // into pixel space
	*/

	// Calculate coordinate for zoom coordinate
	glm::vec2 pixelZoomCoordinate = _coordinate * webViewPixels;

	// Decide whether zooming is finished
	bool finished = false;
	if (!tabInput.gazeUsed && tabInput.instantInteraction) // user demands on instant interaction
	{
		// Set coordinate in output value. Use current gaze position
		SetOutputValue("coordinate", pixelGazeCoordinate);

		// Return success
		finished = true;
	}
	else if (_doDebugging) // TODO debugging
	{
		_logZoom = 1.f;
		_coordinateCenterOffset = glm::vec2(0, 0);
	}
	else
	{
		if (!_driftCorrection) // still refining zoom coordinate
		{
			// Zoom is deep enough for zoom coordinate
			if (_logZoom <= 0.75f)
			{
				_zoomData.logZoom = _logZoom;
				_zoomData.pixelGazeCoordinate = pixelGazeCoordinate;
				_zoomData.pixelZoomCoordinate = pixelZoomCoordinate;
				_driftCorrection = true;
			}
		}
		else // trying to correct gaze drift
		{
			if (_logZoom <= 0.5f)
			{
				// TODO check whether drift is significant or can be ignored (for very good calibration)

				// Calculate drift of gaze
				glm::vec2 pixelGazeDrift = pixelGazeCoordinate - _zoomData.pixelGazeCoordinate; // drift of gaze coordinates

				// Radius around old zoom coordinate where actual fixation point should be
				float pixelRadius = glm::length(pixelGazeDrift) / (_zoomData.logZoom - _logZoom);

				// Calculate fixation coordinate with corrected drift
				glm::vec2 pixelFixationCoordinate = (glm::normalize(pixelGazeDrift) * pixelRadius) + _zoomData.pixelGazeCoordinate;

				// Set coordinate in output value 
				SetOutputValue("coordinate", pixelFixationCoordinate);

				// Return success
				// finished = true; // TODO debugging
				_doDebugging = true;
			}
		}
	}

	// ### Update WebView ###

	// Decrement dimming
	_dimming += tpf;
	_dimming = glm::min(_dimming, DIMMING_DURATION);

	// Tell web view about zoom and dimming
	WebViewParameters webViewParameters;
	webViewParameters.centerOffset = _coordinateCenterOffset;
	webViewParameters.zoom = _logZoom;
	webViewParameters.zoomPosition = _coordinate;
	if (_doDimming) { webViewParameters.dim = DIMMING_VALUE * (_dimming / DIMMING_DURATION); }
	_pTab->SetWebViewParameters(webViewParameters);

    // Return whether finished
    return finished;
}

void ZoomCoordinateAction::Draw() const
{
	// Pixels in web view
	int webViewWidth = _pTab->GetWebViewWidth();
	int webViewHeight = _pTab->GetWebViewHeight();
	glm::vec2 webViewPixels(webViewWidth, webViewHeight);

	// Function to move coordinate according to current zoom. Takes relative coordinate
	const std::function<void(glm::vec2&)> applyZooming = [&](glm::vec2& rRelativeCoordinate)
	{
		rRelativeCoordinate -= _coordinate;
		rRelativeCoordinate /= _logZoom;
		rRelativeCoordinate += _coordinate;
		rRelativeCoordinate -= _coordinateCenterOffset;
	};

	// Zoom coordinate
	glm::vec2 zoomCoordinate(_coordinate); // relative coordiante
	applyZooming(zoomCoordinate);
	_pTab->Debug_DrawRectangle(zoomCoordinate * webViewPixels, glm::vec2(5, 5), glm::vec3(1, 0, 0));

	// Draw info after click would have been performed
	if (_doDebugging)
	{
		glm::vec2 pixelFixationCoordinate;
		GetOutputValue("coordinate", pixelFixationCoordinate);
		_pTab->Debug_DrawRectangle(pixelFixationCoordinate, glm::vec2(5, 5), glm::vec3(1, 1, 0));

		// Line
		_pTab->Debug_DrawLine(zoomCoordinate * webViewPixels, pixelFixationCoordinate, glm::vec3(1, 0, 1));
	}
}

void ZoomCoordinateAction::Activate()
{
	// Nothing to do
}

void ZoomCoordinateAction::Deactivate()
{
	// Reset web view (necessary because of dimming)
	WebViewParameters webViewParameters;
	_pTab->SetWebViewParameters(webViewParameters);
}

void ZoomCoordinateAction::Abort()
{

}
