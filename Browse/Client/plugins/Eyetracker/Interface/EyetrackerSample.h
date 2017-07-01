//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#ifndef EYETRACKERSAMPLE_H_
#define EYETRACKERSAMPLE_H_

#include <chrono>
#include <memory>
#include <deque>

// Struct of sample data
struct SampleData
{
	// Constructor
	SampleData(double x, double y, std::chrono::milliseconds timestamp) : x(x), y(y), timestamp(timestamp)
	{};

	// Fields
	double x;
	double y;
	std::chrono::milliseconds timestamp;
};

// Typedef for unique pointer of sample queue
typedef std::shared_ptr<std::deque<SampleData> > SampleQueue;

#endif EYETRACKERSAMPLE_H_