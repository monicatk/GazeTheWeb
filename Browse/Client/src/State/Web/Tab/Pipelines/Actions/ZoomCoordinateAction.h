//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Action to zoom to a coordinate with gaze.
// - Input: none
// - Output: vec2 coordinate in WebViewPixel space

#ifndef ZOOMCOORDINATEACTION_H_
#define ZOOMCOORDINATEACTION_H_

#include "src/State/Web/Tab/Pipelines/Actions/Action.h"
#include "src/Utils/LerpValue.h"
#include <deque>

class ZoomCoordinateAction : public Action
{
public:

    // Constructor
    ZoomCoordinateAction(TabInteractionInterface* pTab, bool doDimming = true);

    // Update retuns whether finished with execution
    virtual bool Update(float tpf, TabInput tabInput);

    // Draw
    virtual void Draw() const;

    // Activate
    virtual void Activate();

    // Deactivate
    virtual void Deactivate();

    // Abort
    virtual void Abort();

protected:

	// Zoom data at point in time of execution
	struct ZoomData
	{
		glm::vec2 pixelGazeCoordinate; // gaze on web view in pixels. Already free of zooming and center moving offset
		glm::vec2 pixelZoomCoordinate; // coordinate of zooming in pixels
		float logZoom; // value of log zoom at that time
	};

	// Dimming duration
	const float DIMMING_DURATION = 0.5f; // seconds until it is dimmed

	// Dimming value
	const float DIMMING_VALUE = 0.3f;

	// Deviation fading duration (how many seconds until full deviation is back to zero)
	const float DEVIATION_FADING_DURATION = 1.0f;

	// Multiplier of movement towards center
	const float CENTER_OFFSET_MULTIPLIER = 0.0f; // TODO debugging 0.5f;

	// Duration to replace current coordinate with input
	const float MOVE_DURATION = 0.15f;

	// Speed of zoom
	const float ZOOM_SPEED = 0.4f;

    // Coordinate which is updated and later output. In relative web view space
    glm::vec2 _coordinate; // aka zoom coordinate

    // Log zooming amount (used for rendering)
	// Calculated as 1.f - log(_linZoom), so becoming smaller at higher zoom levels
    float _logZoom = 1.f; // [0..1]

    // Linear zooming amount (used for calculations)
	// Increasing while zooming
    float _linZoom = 1.f; // [1..]

    // Offset to center of webview
    glm::vec2 _coordinateCenterOffset = glm::vec2(0, 0);

    // Bool to indicate first update
    bool _firstUpdate = true;

	// Deviation of coordinate (not of gaze!; relative coordiantes, no pixels!)
	// Not really in relative coordinates, since aspect ratio corrected...
	float _deviation = 0.f; // [0..1]

	// Dimming
	float _dimming = 0.f;

	// Do dimming
	bool _doDimming = true;

	// In drift correction mode (while off, zoom coordinate is improved. while on, just zooming happens)
	bool _driftCorrection = false;

	// Datas saved before drift correction zooming starts
	ZoomData _zoomData;

	// TODO: debugging
	bool _doDebugging = false;
};

#endif // ZOOMCOORDINATEACTION_H_
