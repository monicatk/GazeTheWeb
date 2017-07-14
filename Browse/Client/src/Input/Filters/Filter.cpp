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
	if (!spSamples->empty())
	{
		// Update timestamp
		if (!spSamples->empty())
		{
			_timestamp = spSamples->back().timestamp; // should be newest sample
			_timestampSetOnce = true;
		}

		// Move samples over to member
		_spSamples->insert(_spSamples->end(),
			std::make_move_iterator(spSamples->begin()),
			std::make_move_iterator(spSamples->end()));

		// Delete front of queue to match maximum allowed queue length (delete oldest samples)
		int size = (int)_spSamples->size(); // sample queue size
		int overlap = size - setup::FILTER_MEMORY_SIZE;
		for (int i = 0; i < overlap; i++)
		{
			_spSamples->pop_front();
		}
	}
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