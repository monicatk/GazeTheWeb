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
	std::unique_ptr<LabStreamOutput<double> > upLabStreamOutput = nullptr;
	bool doLabStream = true;

	void SetupLabStream(lsl::stream_info streamInfo)
	{
		upLabStreamOutput = std::unique_ptr<LabStreamOutput<double> >(new LabStreamOutput<double>(streamInfo));
	}

	void ContinueLabStream()
	{
		doLabStream = true;
	}

	void PauseLabStream()
	{
		doLabStream = false;
	}

    void PushBackSample(SampleData sample) // called by eye tracker thread
    {
		// Send to LabStreamingLayer
		if (upLabStreamOutput && doLabStream)
		{
			upLabStreamOutput->Send({ sample.x, sample.y });
		}

		// Push sample to queue
		sampleDataMutex.lock(); // lock
		spSampleDataQueue->push_back(sample); // pushes sample
		sampleDataMutex.unlock(); // unlock
    }

	void FetchSamples(SampleQueue& rspSamples) // called by main thread
    {
		sampleDataMutex.lock(); // lock
		rspSamples = spSampleDataQueue; // move data to output
		spSampleDataQueue = SampleQueue(new std::deque<SampleData>); // intialize empty queue for new data
		sampleDataMutex.unlock(); // unlock
    }
}
