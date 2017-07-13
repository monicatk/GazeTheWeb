//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "WeightedAverageFilter.h"
#include "src/Utils/Helper.h"
#include "src/Setup.h"

#include <iostream>

WeightedAverageFilter::WeightedAverageFilter(bool outlierDetection, FilterKernel kernel, unsigned int windowSize) :
	Filter(outlierDetection), _kernel(kernel), _windowSize(windowSize)
{
	// Calculate sigma for gaussian filter
	float sigma = glm::sqrt(-glm::pow(_windowSize - 2.f, 2.f) / (2.f * glm::log(0.05f))); // determine sigma, so that no weight is below 0.05

	// Store denominator of exponential function
	_gaussianDenominator = (2.f * glm::pow(sigma, 2.f));
}

void WeightedAverageFilter::Update(SampleQueue spSamples)
{
	// Super call which moves samples to member
	Filter::Update(spSamples);

	// Go over samples and smooth
	double sumX = 0;
	double sumY = 0;
	double weightSum = 0;
	int oldestUsedIndex = -1; // used for determining fixation duration
	const int size = (int)_spSamples->size();
	const int minIndex = glm::max(0, size - (int)_windowSize);
	int weightIndex = 0; // weight index inputted into weight calculation
	for(int i = size - 1; i >= minIndex; i--) // newest to oldest means reverse order in queue
	{
		// Get sample
		const auto& rGaze = _spSamples->at(i);

		// Saccade detection
		if (oldestUsedIndex >= 0) // at least on loop execution was performed
		{
			// Check distance of current sample and previous one
			const auto& prevGaze = _spSamples->at(i + 1);
			if (glm::distance(
				glm::vec2(prevGaze.x, prevGaze.y),
				glm::vec2(rGaze.x, rGaze.y))
						> setup::FILTER_GAZE_FIXATION_PIXEL_RADIUS)
			{
				break;
			}
		}

		// Calculate weight
		double weight = CalculateWeight(weightIndex);

		// Sum values
		sumX += rGaze.x * weight;
		sumY += rGaze.y * weight;

		// Sum weight
		weightSum += weight;

		// Update index of oldest sample point considered as belonging to this fixation
		oldestUsedIndex = i;

		// Update weight index
		weightIndex++;
	}

	// Update members
	float fixationDuration = 0; // fallback value of fixation duration
	if (oldestUsedIndex >= 0)
	{
		// Filter gaze
		_gazeX = sumX / weightSum;
		_gazeY = sumY / weightSum;

		// Calculate fixation duration
		fixationDuration = (float)((double)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch() - _spSamples->at(oldestUsedIndex).timestamp).count() / 1000.0);
	}
	_fixationDuration = fixationDuration; // update fixation duration
}

double WeightedAverageFilter::GetFilteredGazeX() const
{
	return _gazeX;
}

double WeightedAverageFilter::GetFilteredGazeY() const
{
	return _gazeY;
}

double WeightedAverageFilter::GetRawGazeX() const
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

double WeightedAverageFilter::GetRawGazeY() const
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

float WeightedAverageFilter::GetFixationDuration() const
{
	return _fixationDuration;
}

double WeightedAverageFilter::CalculateWeight(unsigned int i) const
{
	switch (_kernel)
	{
	case FilterKernel::LINEAR:
		return 1;
		break;
	case FilterKernel::TRIANGULAR:
		return (int)_windowSize - (int)i;
		break;
	case FilterKernel::GAUSSIAN:
		return glm::exp(-glm::pow(i, 2.f) / _gaussianDenominator);
		break;
	}
	return 1;
}