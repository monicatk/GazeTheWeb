//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "SimpleFilter.h"
#include "src/Utils/Helper.h"
#include "src/Setup.h"

void SimpleFilter::Update(SampleQueue spSamples)
{
	// Super call which moves samples to member
	Filter::Update(spSamples);

	// Go over samples and smooth
	double filteredGazeX = 0;
	double filteredGazeY = 0;
	double sumX = 0;
	double sumY = 0;
	int filterCount = 0;
	int oldestUsedIndex = -1;
	int size = (int)_spSamples->size();
	for(int i = size - 1; i >= 0; i--) // newest to oldest means reverse order in queue
	{
		// Get sample
		const auto& rGaze = _spSamples->at(i);

		// Check whether sample is within same fixation
		if (filterCount > 0)
		{
			// Calculate filtered gaze
			filteredGazeX = sumX / filterCount;
			filteredGazeY = sumY / filterCount;

			// Check distance of new sample to current filtered gaze
			if (glm::distance(
				glm::vec2(filteredGazeX, filteredGazeY),
				glm::vec2(rGaze.x, rGaze.y))
						> setup::FILTER_GAZE_FIXATION_PIXEL_RADIUS)
			{
				break; // distance too big, break smoothing
			}
		}

		// Sum values
		sumX += rGaze.x;
		sumY += rGaze.y;

		// Increase count of filtered count
		filterCount++;

		// Update index of oldest sample point considered as belonging to this fixation
		oldestUsedIndex = i;
	}

	// Update member if samples where filtered
	if (filterCount > 0)
	{
		// Calculate (final) filtered gaze
		filteredGazeX = sumX / filterCount;
		filteredGazeY = sumY / filterCount;

		// Set member
		_gazeX = filteredGazeX;
		_gazeY = filteredGazeY;
	}

	// Update fixation duration
	float fixationDuration = 0;
	if (oldestUsedIndex >= 0)
	{
		fixationDuration = (float)((double)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch() - _spSamples->at(oldestUsedIndex).timestamp).count() / 1000.0);
	}
	_fixationDuration = fixationDuration;
}

double SimpleFilter::GetFilteredGazeX() const
{
	return _gazeX;
}

double SimpleFilter::GetFilteredGazeY() const
{
	return _gazeY;
}

double SimpleFilter::GetRawGazeX() const
{
	if (!_spSamples->empty())
	{
		return _spSamples->back().x;
	}
	else
	{
		return -1;
	}
}

double SimpleFilter::GetRawGazeY() const
{
	if (!_spSamples->empty())
	{
		return _spSamples->back().y;
	}
	else
	{
		return -1;
	}
}

float SimpleFilter::GetFixationDuration() const
{
	return _fixationDuration;
}