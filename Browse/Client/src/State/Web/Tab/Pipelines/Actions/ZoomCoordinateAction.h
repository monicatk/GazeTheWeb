//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Action to zoom to a coordinate with gaze.

// Notes
// - Input: none
// - Output: vec2 coordinate

#ifndef ZOOMCOORDINATEACTION_H_
#define ZOOMCOORDINATEACTION_H_

#include "Action.h"
#include "src/Utils/LerpValue.h"

class ZoomCoordinateAction : public Action
{
public:

    // Constructor
    ZoomCoordinateAction(TabInteractionInterface* pTab);

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

	// Dimming duration
	const float DIMMING_DURATION = 0.5f; // seconds until it is dimmed

	// Dimming value
	const float DIMMING_VALUE = 0.25f;

	// Deviation fading duration (how many seconds until full deviation is back to zero)
	const float DEVIATION_FADING_DURATION = 0.25f;

	// Multiplier of movement towards center
	const float CENTER_OFFSET_MULTIPLIER = 0.5f;

	// Speed of zoom
	const float ZOOM_SPEED = 0.5f;

    // Coordinate which is updated and output
    glm::vec2 _coordinate;

    // Log zooming amount (used for rendering)
	// Calculated as 1.f - log(_linZoom), so becoming smaller at higher zoom levels
    float _logZoom = 1.f;

    // Linear zooming amout (used for calculations)
	// Increasing while zooming
    float _linZoom = 1.f;

    // Offset to center of webview
    glm::vec2 _coordinateCenterOffset;

    // Bool to indicate first update
    bool _firstUpdate = true;

	// Deviation of coordinate (not of gaze!; relative coordiantes, no pixels!)
	float _deviation = 0; // [0..1]

	// Dimming
	float _dimming = 0.f;
};

#endif // ZOOMCOORDINATEACTION_H_
