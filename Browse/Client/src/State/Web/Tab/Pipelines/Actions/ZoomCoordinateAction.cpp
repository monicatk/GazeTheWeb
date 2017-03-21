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
	// ### Preparation ###

	// Speed of zooming
	float zoomSpeed = 0.f;

	// Pixels in cef pixels
	glm::vec2 cefPixels(_pTab->GetWebViewResolutionX(), _pTab->GetWebViewResolutionY());

	// Function transforms from relative WebView coordinates to CEFPixel coordinates
	const std::function<void(glm::vec2&)> pageCoordinate = [&](glm::vec2& rRelativeCoordinate)
	{
		// Analogous to shader in WebView
		rRelativeCoordinate += _relativeCenterOffset; // add center offset
		rRelativeCoordinate -= _relativeZoomCoordinate; // move zoom position to origin
		rRelativeCoordinate *= _logZoom; // apply zoom
		rRelativeCoordinate += _relativeZoomCoordinate; // move back
		rRelativeCoordinate *= cefPixels; // into pixel space of CEF
	};

	// Current gaze
	glm::vec2 relativeGazeCoordiante = glm::vec2(tabInput.webViewGazeRelativeX, tabInput.webViewGazeRelativeY);
	glm::vec2 pixelGazeCoordinate = relativeGazeCoordiante; // CEFPixel space
	pageCoordinate(pixelGazeCoordinate);

	// ### Update zoom speed and center offset ###

	// Only allow zoom in when gaze upon web view and not yet used
	if (!tabInput.gazeUsed && tabInput.insideWebView) // TODO: gazeUsed really good idea here? Maybe later null pointer?
	{
		switch (_state)
		{
		case State::ORIENTATE:
		{
			// Update deviation value (fade away deviation)
			_deviation = glm::max(_deviation - (tpf / DEVIATION_FADING_DURATION), 0.f);

			// Update coordinate
			if (!_firstUpdate)
			{
				// Move towards new coordinate
				glm::vec2 relativeDelta = (relativeGazeCoordiante + _relativeCenterOffset) - _relativeZoomCoordinate; // TODO: why + relativeCenterOffset?
				_relativeZoomCoordinate += relativeDelta * glm::min(1.f, (tpf / MOVE_DURATION));

				// Set length of delta to deviation if bigger than current deviation
				_deviation = glm::min(1.f, glm::max(glm::length(relativeDelta), _deviation)); // [0..1]

				// If at the moment a high deviation is given, try to zoom out to give user more overview
				zoomSpeed = ZOOM_SPEED - glm::min(1.f, 3.f * _deviation); // TODO weight deviation more intelligent
			}
			else // first frame of execution
			{
				// Use current gaze coordinate as new coordinate
				_relativeZoomCoordinate = relativeGazeCoordiante;

				// Since only for first frame, do not do it again
				_firstUpdate = false;
			}

			// Calculated center offset
			_relativeCenterOffset =
				CENTER_OFFSET_MULTIPLIER
				* (1.f - _logZoom) // weight with zoom (starting at zero) to have more centered version at higher zoom level
				* (_relativeZoomCoordinate - 0.5f); // vector from current zoom coordinate to center of web view

			// Get out of case
			break;
		}
		case State::ZOOM:
			zoomSpeed = 0.25f;
			break;
		default:
			zoomSpeed = 0.f;
			break;
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

	// Decide whether zooming is finished
	bool finished = false;
	/*
	if (!tabInput.gazeUsed && tabInput.instantInteraction) // user demands on instant interaction
	{
		// Set coordinate in output value. Use current gaze position
		// SetOutputValue("coordinate", pixelGazeCoordinate); // TODO: Debugging

		// Return success
		finished = true;
	}

	// Proceed depending on the state
	switch (_state)
	{
		case State::ORIENTATE:
		{
			// Zoom is deep enough for zoom coordinate
			if (_logZoom <= 0.75f)
			{
				_zoomData.logZoom = _logZoom;
				_zoomData.pixelGazeCoordinate = pixelGazeCoordinate;
				_zoomData.pixelZoomCoordinate = pixelZoomCoordinate;
				// _state = State::ZOOM;
			}
			break;
		}
		case State::ZOOM:
		{
			if (_logZoom <= 0.5f)
			{
				_state = State::WAIT;
			}
			break;
		}
		case State::WAIT:
		{
			_gazeCalmDownTime -= tpf;
			if (_gazeCalmDownTime < 0)
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
				_state = State::DEBUG;
			}
			break;
		}
		case State::DEBUG:
		{
			_logZoom = 1.f;
			_relativeCenterOffset = glm::vec2(0, 0);
			break;
		}
	}
	*/

	// ### Update WebView ###

	// Decrement dimming
	_dimming += tpf;
	_dimming = glm::min(_dimming, DIMMING_DURATION);

	// Tell web view about zoom and dimming
	WebViewParameters webViewParameters;
	webViewParameters.centerOffset = _relativeCenterOffset;
	webViewParameters.zoom = _logZoom;
	webViewParameters.zoomPosition = _relativeZoomCoordinate;
	if (_doDimming) { webViewParameters.dim = DIMMING_VALUE * (_dimming / DIMMING_DURATION); }
	_pTab->SetWebViewParameters(webViewParameters);

    // Return whether finished
    return finished;
}

void ZoomCoordinateAction::Draw() const
{
	// WebView pixels
	glm::vec2 webViewPixels(_pTab->GetWebViewWidth(), _pTab->GetWebViewHeight());

	// Function to move coordinate according to current zoom. Takes relative WebView coordinate
	const std::function<void(glm::vec2&)> applyZooming = [&](glm::vec2& rCoordinate)
	{
		rCoordinate -= _relativeZoomCoordinate;
		rCoordinate /= _logZoom; // inverse to WebView shader
		rCoordinate += _relativeZoomCoordinate;
		rCoordinate -= _relativeCenterOffset; // inverse to WebView shader
		rCoordinate *= webViewPixels;
	};

	// Zoom coordinate
	glm::vec2 zoomCoordinate(_relativeZoomCoordinate);
	applyZooming(zoomCoordinate);
	_pTab->Debug_DrawRectangle(zoomCoordinate, glm::vec2(5, 5), glm::vec3(1, 0, 0));

	// Testing visualization
	glm::vec2 testCoordinate(0.3f, 0.5f);
	applyZooming(testCoordinate);
	_pTab->Debug_DrawRectangle(testCoordinate, glm::vec2(5, 5), glm::vec3(0, 0, 1));
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
