//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Action to magnify to a coordinate with gaze.
// - Input: none
// - Output: vec2 coordinate in CEFPixel space

#ifndef MAGNIFICATIONCOORDINATEACTION_H_
#define MAGNIFICATIONCOORDINATEACTION_H_

#include "src/State/Web/Tab/Pipelines/Actions/Action.h"
#include "src/Utils/LerpValue.h"
#include <deque>

class MagnificationCoordinateAction : public Action
{
public:

    // Constructor
	MagnificationCoordinateAction(TabInteractionInterface* pTab, bool doDimming = true);

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
	const float DIMMING_VALUE = 0.3f;

	// Deviation fading duration (how many seconds until full deviation is back to zero)
	const float DEVIATION_FADING_DURATION = 1.0f;

	// Deviation weight on zoom speed
	const float DEVIATION_WEIGHT = 3.0f;

	// Level of magnification
	const float MAGNIFICATION = 0.5f;

	// Magnfication center. In relative page space
	glm::vec2 _relativeMagnificationCenter;

	// Variable to indicate whether magnified or not
	bool _magnified = false;

	// Dimming
	float _dimming = 0.f;

	// Do dimming
	bool _doDimming = true;
};

#endif // MAGNIFICATIONCOORDINATEACTION_H_
