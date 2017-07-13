//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Weighted average filtering.

#ifndef WEIGHTEDAVERAGEFILTER_H_
#define WEIGHTEDAVERAGEFILTER_H_

#include "src/Input/Filters/Filter.h"
#include "src/Input/Filters/FilterKernel.h"

class WeightedAverageFilter : public Filter
{
public:

	// Constructor
	WeightedAverageFilter(
		bool outlierDetection, // whether outlier detection is used, delays input by one sample
		FilterKernel kernel, // type of weights used
		unsigned int windowSize); // count of samples used for filtering

	// Update. Takes samples in window pixel coordinates. Samples are moved out provided variable
	virtual void Update(SampleQueue spSamples) override;

	// Various getters
	virtual double GetFilteredGazeX() const override;
	virtual double GetFilteredGazeY() const override;
	virtual double GetRawGazeX() const override;
	virtual double GetRawGazeY() const override;
	virtual float GetFixationDuration() const override;

private:

	// Calulcate weight for a sample. Takes "oldness" of sample.
	// Interval must be [0.._windowSize-1]
	double CalculateWeight(unsigned int i) const;

	// Members
	double _gazeX = -1; // filtered
	double _gazeY = -1; // filtered
	float _fixationDuration = 0;
	FilterKernel _kernel;
	unsigned int _windowSize;
	float _gaussianDenominator = 0.f;
};

#endif WEIGHTEDAVERAGEFILTER_H_