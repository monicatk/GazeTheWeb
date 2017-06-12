//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Handles data from eye trackers. As fetch is called from main thread and
// push back from eye trackers, mutex is used for synchronisation.

#ifndef EYETRACKERDATA_H_
#define EYETRACKERDATA_H_

#include "plugins/Eyetracker/Interface/EyetrackerSample.h"
#include <vector>

namespace eyetracker_global
{
	void PushBackSample(SampleData sample);
	void FetchSamples(SampleQueue& rupSamples);
}

#endif EYETRACKERDATA_H_
