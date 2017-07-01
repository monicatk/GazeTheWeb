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

	// Constructor
	SimpleFilter();

	// Update. Takes samples in window pixel coordinates
	virtual void Update(SampleQueue spSamples) override;

	// Various getters
	virtual double GetFilteredGazeX() const override;
	virtual double GetFilteredGazeY() const override;
	virtual double GetRawGazeX() const override;
	virtual double GetRawGazeY() const override;
	virtual bool IsSaccade() const override;

private:

	// Testing
	double _gazeX = -1; // filtered
	double _gazeY = -1; // filtered
	bool _saccade = false;
	SampleQueue _spSamples;
};

#endif SIMPLEFILTER_H_