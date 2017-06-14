//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "Filter.h"

#include "src/Global.h"
#include <algorithm>

void Filter::Update(SampleQueue spSamples)
{
	// Update timestamp
	if (!spSamples->empty())
	{
		_timestamp = spSamples->back().timestamp; // should be newest sample
		_timestampSetOnce = true;
	}
}

float Filter::GetAge() const
{
	if (_timestampSetOnce)
	{
		using namespace std::chrono;
		return std::min(
			FILTER_MAXIMUM_SAMPLE_AGE,
			(float)((double)std::chrono::duration_cast<std::chrono::milliseconds>(system_clock::now().time_since_epoch() - _timestamp).count() / 1000.0));
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

