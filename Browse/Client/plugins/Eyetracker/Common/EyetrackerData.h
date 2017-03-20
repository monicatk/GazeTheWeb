//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// As the eye tracker call a Windows level callback, the whole application
// or at least the main thread freezes and waits for it to receive and
// process gaze data. Because in main application, you can never be sure
// when the gaze data is updated, an array of const length is used to store
// received data. Everytime new samples arrive, the array is filled behind the
// current index and the index is updated.

#include <vector>

static const int EYETRACKER_SAMPLE_COLLECTION_COUNT = 120;

namespace eyetracker_global
{
	void PushBackRawData(double gazeX, double gazeY, bool valid);
	void GetKOrLessValidRawGazeEntries(int k, std::vector<double>& rGazeX, std::vector<double>& rGazeY);
}
