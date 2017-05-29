//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#ifndef EYETRACKERSAMPLEDATA_H_
#define EYETRACKERSAMPLEDATA_H_

#include <chrono>

// Struct of sample data
struct SampleData
{
	// Constructor
	SampleData(double x, double y, bool valid, std::chrono::milliseconds timestamp) : x(x), y(y), valid(valid), timestamp(timestamp)
	{};

	// Fields
	double x;
	double y;
	bool valid;
	std::chrono::milliseconds timestamp;
};

#endif EYETRACKERSAMPLEDATA_H_