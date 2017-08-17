//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "Filter.h"

#include "src/Global.h"
#include "src/Setup.h"
#include <algorithm>

Filter::Filter() :
	_spSamples(SampleQueue(new std::deque<SampleData>))
{
	// Nothing to do
}

Filter::~Filter()
{
	// For the sake of the C++ standard
}

void Filter::Update(SampleQueue spSamples)
{
	// Only work with non-empty sample queue
	if (!spSamples->empty())
	{
		// Update timestamp
		_timestamp = spSamples->back().timestamp; // should be newest sample
		_timestampSetOnce = true;

		// Move samples over to member
		_spSamples->insert(_spSamples->end(),
			std::make_move_iterator(spSamples->begin()),
			std::make_move_iterator(spSamples->end()));
	}
	
	// Delete front of queue to match maximum allowed queue length (delete oldest samples)
	int size = (int)_spSamples->size(); // sample queue size
	int overlap = size - setup::FILTER_MEMORY_SIZE;
	for (int i = 0; i < overlap; i++)
	{
		_spSamples->pop_front();
	}

	// Apply filtering to retrieve current filtered gaze coordinate and other information
	ApplyFilter(_spSamples, _gazeX, _gazeY, _fixationDuration);
}

double Filter::GetRawGazeX() const
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

double Filter::GetRawGazeY() const
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

double Filter::GetFilteredGazeX() const
{
	return _gazeX;
}

double Filter::GetFilteredGazeY() const
{
	return _gazeY;
}

float Filter::GetFixationDuration() const
{
	return _fixationDuration;
}

float Filter::GetAge() const
{
	if (_timestampSetOnce)
	{
		return std::min(
			FILTER_MAXIMUM_SAMPLE_AGE,
			(float)((double)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch() - _timestamp).count() / 1000.0));
	}
	else
	{
		return FILTER_MAXIMUM_SAMPLE_AGE;
	}
}

bool Filter::IsTimestampSetOnce() const
{
	return _timestampSetOnce;
}