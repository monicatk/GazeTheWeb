//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Master interface for accessing functionality from other threads.

#ifndef MASTERTHREADSAFEINTERFACE_H_
#define MASTERTHREADSAFEINTERFACE_H_

#include "src/Input/EyeTrackerStatus.h"

class MasterThreadsafeInterface
{
public:

	// TODO: make better job queue, this is the best solution for thread safeness!

	// Notify about eye tracker status
	virtual void threadsafe_EyeTrackerStatusNotification(EyeTrackerStatus status) = 0;
};

#endif // MASTERTHREADSAFEINTERFACE_H_
