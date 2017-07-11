//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Simple filtering of gaze data.

#ifndef SIMPLEFILTER_H_
#define SIMPLEFILTER_H_

#include "src/Input/Filters/Filter.h"

class SimpleFilter : public Filter
{
public:

	// Update. Takes samples in window pixel coordinates. Samples are moved out provided variable
	virtual void Update(SampleQueue spSamples) override;

	// Various getters
	virtual double GetFilteredGazeX() const override;
	virtual double GetFilteredGazeY() const override;
	virtual double GetRawGazeX() const override;
	virtual double GetRawGazeY() const override;
	virtual float GetFixationDuration() const override;

private:

	// Testing
	double _gazeX = -1; // filtered
	double _gazeY = -1; // filtered
	float _fixationDuration = 0;
};

#endif SIMPLEFILTER_H_