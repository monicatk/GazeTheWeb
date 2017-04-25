//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Action to zoom to a coordinate with gaze.
// - Input: none
// - Output: vec2 coordinate in CEFPixel space

// TODO: this version is from master branch, so quite outdated!

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

	// Struct which is used to record data while execution
	struct ZoomData
	{
		float lifetime = 0.f; // time since data was created
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
	const float CENTER_OFFSET_MULTIPLIER = 0.5f;

	// Duration to replace current coordinate with input
	const float MOVE_DURATION = 0.75f;

	// Speed of zoom
	const float ZOOM_SPEED = 0.4f;

	// Maximal time which is looked back at compensation of calibration errors
	const float LOOK_BACK_TIME = 0.5f;

	// Maximal angle between gaze drift and zoom coordinate drift in degree
	const float DRIFT_MAX_ANGLE_DEGREE = 10.f;

    // Coordinate which is updated and later output. In relative page space
    glm::vec2 _relativeZoomCoordinate;

    // Log zooming amount (used for rendering)
	// Calculated as 1.f - log(_linZoom), so becoming smaller at higher zoom levels
    float _logZoom = 1.f;

    // Linear zooming amout (used for calculations)
	// Increasing while zooming
    float _linZoom = 1.f;

    // Offset to center of webview
    glm::vec2 _coordinateCenterOffset = glm::vec2(0, 0);

    // Bool to indicate first update
    bool _firstUpdate = true;

	// Deviation of coordinate (not of gaze!; relative coordiantes, no pixels!)
	// Not really in relative coordinates, since aspect ratio corrected...
	float _deviation = 0.f; // [0..1]

	// Dimming
	float _dimming = 0.f;

	// Queue for recording data which is used as click to compensate calibration errors
	std::deque<ZoomData> _zoomDataQueue;

	// Do dimming
	bool _doDimming = true;
};

#endif // ZOOMCOORDINATEACTION_H_
