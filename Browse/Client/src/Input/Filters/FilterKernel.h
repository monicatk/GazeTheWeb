//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Filter kernel enumeration.

#ifndef FILTERKERNEL_H_
#define FILTERKERNEL_H_

enum class FilterKernel
{
	LINEAR, // weight is always one
	TRIANGULAR, // weights least recent point with 1 and next with 2, then 3 and so on
	GAUSSIAN // applies gaussian function value
};

#endif // FILTERKERNEL_H_