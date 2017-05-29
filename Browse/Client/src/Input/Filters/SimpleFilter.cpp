//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "SimpleFilter.h"

void SimpleFilter::Update(std::vector<SampleData> samples,
	double& rGazeX,
	double& rGazeY,
	bool& rSaccade)
{
	// TODO: Implement fancy filtering
	if (!samples.empty())
	{
		_gazeX = samples.back().x;
		_gazeY = samples.back().y;
	}
	rGazeX = _gazeX;
	rGazeY = _gazeY;
	rSaccade = false;
}