//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "EyetrackerData.h"
#include <mutex>

namespace eyetracker_global
{
    // Variables
	auto upSampleDataQueue = SampleQueue(new std::deque<SampleData>);
	std::mutex sampleDataMutex;

    void PushBackSample(SampleData sample)
    {
		sampleDataMutex.lock(); // lock
		upSampleDataQueue->push_back(sample); // pushes sample
		sampleDataMutex.unlock(); // unlock
    }

	void FetchSamples(SampleQueue& rupSamples)
    {
		sampleDataMutex.lock(); // lock
		rupSamples = std::move(upSampleDataQueue); // move data to output
		upSampleDataQueue = SampleQueue(new std::deque<SampleData>); // intialize empty queue for new data
		sampleDataMutex.unlock(); // unlock
    }
}
