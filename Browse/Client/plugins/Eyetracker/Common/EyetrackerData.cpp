//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "EyetrackerData.h"
#include <algorithm>
#include <mutex>

namespace eyetracker_global
{
    // Variables
	std::vector<SampleData> sampleDataVector;
	std::mutex sampleDataMutex;

    void PushBackSample(SampleData sample)
    {
		std::lock_guard<std::mutex> guard(sampleDataMutex); // locks mutex before pushing samples
		sampleDataVector.push_back(sample); // pushes sample

		// mutex is unlocked by destruction of guard
    }

	void FetchSamples(std::vector<SampleData>& rSamples)
    {
		std::lock_guard<std::mutex> guard(sampleDataMutex); // locks mutex before working on samples
		rSamples = std::move(sampleDataVector); // move data to output
		sampleDataVector.clear(); // should not be necessary

		// mutex is unlocked by destruction of guard
    }
}
