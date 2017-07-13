//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Abstract interface for eye gaze filters.

#ifndef FILTER_H_
#define FILTER_H_

#include "plugins/Eyetracker/Interface/EyetrackerSample.h"

class Filter
{
public:

	// Constructor
	Filter(bool outlierDetection); // outlier detection discards single sample disturb a fixation

	// Destructor
	virtual ~Filter() = 0;

	// Update. Takes samples in window pixel coordinates. Samples are moved out provided variable
	virtual void Update(SampleQueue spSamples);

	// Various abstract getters
	virtual double GetFilteredGazeX() const = 0;
	virtual double GetFilteredGazeY() const = 0;
	virtual double GetRawGazeX() const = 0;
	virtual double GetRawGazeY() const = 0;
	virtual float GetFixationDuration() const = 0;

	// Getter for age of last used sample
	float GetAge() const;

	// Getter which returns whether timestamp was actively set at least once (aka at least one sample received)
	bool IsTimestampSetOnce() const;

protected:

	// Samples to filter
	SampleQueue _spSamples;

private:

	// Indicator about outlier detection
	bool _outlierDetection;

	// Latest gaze sample is stored here and added with the next update, when outlier correction is performed
	std::unique_ptr<SampleData> _upCandidate = nullptr;

	// Timestamp of last sample
	std::chrono::milliseconds _timestamp;

	// Bool whether timestamp was set at least once (aka at least one sample received)
	bool _timestampSetOnce = false;
};

#endif FILTER_H_