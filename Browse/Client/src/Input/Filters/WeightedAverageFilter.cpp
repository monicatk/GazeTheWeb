//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "WeightedAverageFilter.h"
#include "src/Utils/Helper.h"
#include "src/Setup.h"

#include <iostream>

WeightedAverageFilter::WeightedAverageFilter(FilterKernel kernel, unsigned int windowSize, bool outlierRemoval) :
	_kernel(kernel), _windowSize(windowSize), _outlierRemoval(outlierRemoval)
{
	// Calculate sigma for gaussian filter
	float sigma = glm::sqrt(-glm::pow(_windowSize - 1.f, 2.f) / (2.f * glm::log(0.05f))); // determine sigma, so that no weight is below 0.05

	// Store denominator of exponential function
	_gaussianDenominator = (2.f * glm::pow(sigma, 2.f));
}

void WeightedAverageFilter::Update(SampleQueue spSamples)
{
	// Super call which moves samples to member
	Filter::Update(spSamples);

	// Prepare variables
	double sumX = 0;
	double sumY = 0;
	double weightSum = 0;
	
	// Indexing
	const int size = (int)_spSamples->size();
	int endIndex = glm::max(0, size - (int)_windowSize);
	int startIndex = size - 1;

	// Indices updated by loop
	int weightIndex = 0; // weight index inputted into weight calculation
	int oldestUsedIndex = -1; // used for determining fixation duration

	// Adjust indices for outlier removal (latest sample may not be used)
	if (_outlierRemoval)
	{
		// Queue is iterated from back (where the latest samples are) to the front. So move one entry towards front
		--startIndex; // is -1 for queue with one sample, so single sample is not filtered and nothing happens
		endIndex = glm::max(0, endIndex - 1); // end index must be floored to zero
	}

	// Go over samples and smooth
	for(int i = startIndex; i >= endIndex; --i) // latest to oldest means reverse order in queue
	{
		// Get current sample
		const auto& rGaze = _spSamples->at(i);

		// Saccade detection
		if (i < size - 1) // only proceed when there is a previous sample to check against
		{
			// Check distance of current sample and previously filtered one
			const auto& prevGaze = _spSamples->at(i + 1); // in terms of time, prevGaze is newer than gaze
			if (glm::distance(
				glm::vec2(prevGaze.x, prevGaze.y),
				glm::vec2(rGaze.x, rGaze.y))
				> setup::FILTER_GAZE_FIXATION_PIXEL_RADIUS)
			{
				if (_outlierRemoval) // check whether to ignore this as outlier or really breaking
				{
					int nextIndex = i - 1; // index of next sample to filter (which is older than current)
					if (nextIndex >= 0)
					{
						const auto& nextGaze = _spSamples->at(nextIndex);
						if (glm::distance(
							glm::vec2(prevGaze.x, prevGaze.y),
							glm::vec2(nextGaze.x, nextGaze.y))
							> setup::FILTER_GAZE_FIXATION_PIXEL_RADIUS)
						{
							break; // previous and next sample do not belong to the same fixation, so this sample is start of new fixation
						}
						else
						{
							continue; // previous and next sample do belong to the same fixation, skip this outlier
						}
					}
					else
					{
						break; // no next index available, so cannot deterime whether this is an outlier
					}
					
				}
				else // no outlier removal performed
				{
					break; // just break as this sample is too far away from previous
				}
			}
		}

		// Calculate weight
		double weight = CalculateWeight(weightIndex);

		// Sum values
		sumX += rGaze.x * weight;
		sumY += rGaze.y * weight;

		// Sum weight for later normalization
		weightSum += weight;

		// Update index of oldest sample point considered as belonging to current fixation
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

		// Calculate fixation duration (duration from now to receiving of oldest sample contributing to fixation)
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
		return 1.0;
		break;
	case FilterKernel::TRIANGULAR:
		return (int)_windowSize - (int)i;
		break;
	case FilterKernel::GAUSSIAN:
		return glm::exp(-glm::pow(i, 2.f) / _gaussianDenominator);
		break;
	}
	return 1.0;
}