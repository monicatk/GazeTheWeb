//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "EyetrackerData.h"
#include <mutex>

namespace eyetracker_global
{
    // Variables
	auto spSampleDataQueue = SampleQueue(new std::deque<SampleData>);
	std::mutex sampleDataMutex;

    void PushBackSample(SampleData sample)
    {
		sampleDataMutex.lock(); // lock
		spSampleDataQueue->push_back(sample); // pushes sample
		sampleDataMutex.unlock(); // unlock
    }

	void FetchSamples(SampleQueue& rspSamples)
    {
		sampleDataMutex.lock(); // lock
		rspSamples = spSampleDataQueue; // move data to output
		spSampleDataQueue = SampleQueue(new std::deque<SampleData>); // intialize empty queue for new data
		sampleDataMutex.unlock(); // unlock
    }
}
