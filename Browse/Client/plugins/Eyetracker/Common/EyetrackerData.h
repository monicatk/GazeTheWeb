//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Handles data from eye trackers. As fetch is called from main thread and
// push back from eye trackers, mutex is used for synchronisation.

#include "plugins/Eyetracker/Interface/EyetrackerSampleData.h"
#include <vector>

namespace eyetracker_global
{
	void PushBackSample(SampleData sample);
	void FetchSamples(SampleQueue& rupSamples);
}
