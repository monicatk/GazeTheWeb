//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "EyetrackerData.h"
#include <mutex>

namespace eyetracker_global
{
    // Variables
	auto upSampleDataVector = SampleVector(new std::vector<SampleData>);
	std::mutex sampleDataMutex;

    void PushBackSample(SampleData sample)
    {
		sampleDataMutex.lock(); // lock
		upSampleDataVector->push_back(sample); // pushes sample
		sampleDataMutex.unlock(); // unlock
    }

	void FetchSamples(SampleVector& rupSamples)
    {
		sampleDataMutex.lock(); // lock
		rupSamples = std::move(upSampleDataVector); // move data to output
		upSampleDataVector = SampleVector(new std::vector<SampleData>); // intialize empty vector for new data
		sampleDataMutex.unlock(); // unlock
    }
}
