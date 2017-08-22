//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "MagnificationCoordinateAction.h"
#include "src/State/Web/Tab/Interface/TabInteractionInterface.h"
#include "submodules/glm/glm/gtx/vector_angle.hpp"
#include <algorithm>
#include <iostream>

MagnificationCoordinateAction::MagnificationCoordinateAction(TabInteractionInterface* pTab, bool doDimming) : Action(pTab)
{
	// Save members
	_doDimming = doDimming;

    // Add in- and output data slots
    AddVec2OutputSlot("coordinate");
}

bool MagnificationCoordinateAction::Update(float tpf, const std::shared_ptr<const TabInput> spInput)
{
	// Function transforms coordinate from relative WebView coordinates to CEFPixel coordinates on page
	const std::function<void(const float&, const glm::vec2&, const glm::vec2&, glm::vec2&)> pageCoordinate
		= [&](const float& rZoom, const glm::vec2& rRelativeMagnificationCoordinate, const glm::vec2& rRelativeCenterOffset, glm::vec2& rCoordinate)
	{
		// Analogous to shader in WebView
		rCoordinate += rRelativeCenterOffset; // add center offset
		rCoordinate -= rRelativeMagnificationCoordinate; // move magnification coordinate to origin
		rCoordinate *= rZoom; // apply zoom
		rCoordinate += rRelativeMagnificationCoordinate; // move back
		rCoordinate *= glm::vec2(_pTab->GetWebViewResolutionX(), _pTab->GetWebViewResolutionY()); // bring into pixel space of CEF
	};

	// Current gaze
	glm::vec2 relativeGazeCoordinate = glm::vec2(spInput->webViewRelativeGazeX, spInput->webViewRelativeGazeY); // relative WebView space

	// Values of interest
	glm::vec2 relativeCenterOffset = _magnified ? _relativeMagnificationCenter - glm::vec2(0.5f, 0.5f) : glm::vec2(0);
	float zoom = _magnified ? MAGNIFICATION : 1.f;
	glm::vec2 relativeMagnificationCenter = _relativeMagnificationCenter;

	// Decrease fixationWaitTime
	if (fixationWaitTime > 0)
	{
		fixationWaitTime -= tpf;
		glm::max(fixationWaitTime, 0.f);
	}

	// Decide whether to magnify or to finish
	bool finished = false;
	if (!spInput->gazeUponGUI && (spInput->instantInteraction || (fixationWaitTime <= 0 && spInput->fixationDuration >= FIXATION_DURATION))) // user demands on instant interaction or fixates on the screen
	{
		// Check for magnification
		if (_magnified) // already magnified, so finish this action
		{
			// Set output
			glm::vec2 coordinate = relativeGazeCoordinate;
			pageCoordinate(zoom, relativeMagnificationCenter, relativeCenterOffset, coordinate); // transform gaze relative to WebView to page coordinates
			SetOutputValue("coordinate", coordinate); // into pixel space of CEF

			// Finish this action
			finished = true;
		}
		else // not yet magnified, do it now
		{
			// Set magnification center
			_relativeMagnificationCenter = relativeGazeCoordinate;

			// Remember magnification
			_magnified = true;

			// Reset fixation wait time so no accidential instant interaction after magnification can happen
			fixationWaitTime = FIXATION_DURATION;
		}
	}

	// Decrement dimming
	_dimming += tpf;
	_dimming = glm::min(_dimming, DIMMING_DURATION);

	// Tell web view about zoom
	WebViewParameters webViewParameters;
	webViewParameters.centerOffset = relativeCenterOffset;
	webViewParameters.zoom = zoom;
	webViewParameters.zoomPosition = relativeMagnificationCenter;
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
