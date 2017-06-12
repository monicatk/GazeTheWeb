//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Simple filtering of gaze data.

#ifndef SIMPLEFILTER_H_
#define SIMPLEFILTER_H_

#include "src/Input/Filters/Filter.h"

class SimpleFilter
{
public:

	// Constructor
	SimpleFilter();

	// Update. Takes samples in window pixel coordinates
	virtual void Update(SampleQueue spSamples,
		double& rGazeX,
		double& rGazeY,
		bool& rSaccade);

private:

	// Testing
	double _gazeX = 0;
	double _gazeY = 0;
	SampleQueue _spSamples;
};

#endif SIMPLEFILTER_H_