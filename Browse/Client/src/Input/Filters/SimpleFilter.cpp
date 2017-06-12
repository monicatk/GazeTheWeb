//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "SimpleFilter.h"
#include "src/Utils/Helper.h"
#include "src/Setup.h"

SimpleFilter::SimpleFilter() : _spSamples(SampleQueue(new std::deque<SampleData>))
{
	// Nothing to do
}

void SimpleFilter::Update(SampleQueue spSamples,
	double& rGazeX,
	double& rGazeY,
	bool& rSaccade)
{
	// Move samples over to member
	_spSamples->insert(_spSamples->end(),
		std::make_move_iterator(spSamples->begin()),
		std::make_move_iterator(spSamples->end()));

	// Delete beginning of queue to match maximum allowed queue length
	int size = (int)_spSamples->size(); // sample queue size
	int overlap = size - 100; // TODO: non hardcoded length
	for (int i = 0; i < overlap; i++)
	{
		_spSamples->pop_front();
	}
	size = (int)_spSamples->size(); // update sample queue size

	// Go over samples and filter
	double filteredGazeX = 0;
	double filteredGazeY = 0;
	double sumX = 0;
	double sumY = 0;
	int filterCount = 0;
	for(int i = size - 1; i >= 0; i--) // newest to oldest means reverse order in queue
	{
		// Get sample
		const auto& rGaze = _spSamples->at(i);

		// Check whether new sample is withing same fixation
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
				break;
			}
		}

		// Sum values
		sumX += rGaze.x;
		sumY += rGaze.y;

		// Increase count of filtered count
		filterCount++;
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

	// Fill reference
	rGazeX = _gazeX;
	rGazeY = _gazeY;
	rSaccade = filterCount > 1;
}