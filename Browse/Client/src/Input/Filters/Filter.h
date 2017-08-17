//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Abstract interface for eye gaze filters.

#ifndef FILTER_H_
#define FILTER_H_

#include "plugins/Eyetracker/Interface/EyetrackerSample.h"
#include <functional>
#include <map>

// Typedef for custom transformation
typedef std::function<std::pair<double, double>(double, double)> FilterTransformation;

// Filter class
class Filter
{
public:

	// Constructor
	Filter();

	// Destructor
	virtual ~Filter() = 0;

	// Update. Takes samples in window pixel coordinates. Samples are moved from provided variable
	void Update(SampleQueue spSamples);

	// Various getters
	double GetRawGazeX() const;
	double GetRawGazeY() const;
	double GetFilteredGazeX() const;
	double GetFilteredGazeY() const;
	float GetFixationDuration() const;

	// Getter for age of last used sample
	float GetAge() const;

	// Getter which returns whether timestamp was actively set at least once (aka at least one sample received)
	bool IsTimestampSetOnce() const;

	/*

	// Register custom transformation of gaze before filtering (smooth pursuit fixation).
	// For each custom transformation, retrieved samples are transformed with provided transformation and stored.
	// At registration, provided transformation is applied on all existing samples after copying
	bool RegisterCustomTransformation(std::string name, FilterTransformation transformation); // return whether successful

	// Set transformation for incoming gaze data for the chosen custom transformation
	bool ChangeCustomTransformation(std::string name, FilterTransformation transformation);

	// Unregister custom transformation
	bool UnregisterCustomTransformation(std::string name);

	// Retrieve filtered gaze with custom transformation
	virtual double GetFilteredGazeX(std::string name) const = 0;
	virtual double GetFilteredGazeY(std::string name) const = 0;

	*/

private:

	// Actual implementation of filtering
	virtual void ApplyFilter(const SampleQueue& rSamples, double& rGazeX, double& rGazeY, float& rFixationDuration) const = 0;

	// Timestamp of last sample
	std::chrono::milliseconds _timestamp;

	// Bool whether timestamp was set at least once (aka at least one sample received)
	bool _timestampSetOnce = false;

	// Samples to filter
	SampleQueue _spSamples;

	// Samples of custom transformations
	struct CustomTranformation
	{
		FilterTransformation transformation;
		SampleQueue queue;
		double gazeX = -1;
		double gazeY = -1;
		float fixationDuration = 0;
	};
	std::map<std::string, CustomTranformation> _customTransformationSamples;

	// Filtered stuff
	double _gazeX = -1; // filtered
	double _gazeY = -1; // filtered
	float _fixationDuration = 0;
};

#endif FILTER_H_