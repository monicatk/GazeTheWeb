//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "SimpleFilter.h"

void SimpleFilter::Update(SampleVector upSamples,
	double& rGazeX,
	double& rGazeY,
	bool& rSaccade)
{
	// TODO: Implement fancy filtering
	if (!upSamples->empty())
	{
		_gazeX = upSamples->back().x;
		_gazeY = upSamples->back().y;
	}
	rGazeX = _gazeX;
	rGazeY = _gazeY;
	rSaccade = false;
}