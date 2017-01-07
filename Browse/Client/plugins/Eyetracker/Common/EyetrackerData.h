//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include <vector>

static const int EYETRACKER_SAMPLE_COLLECTION_COUNT = 120;

namespace eyetracker_global
{
	void PushBackRawData(double gazeX, double gazeY, bool valid);
	void GetKOrLessValidRawGazeEntries(int k, std::vector<double>& rGazeX, std::vector<double>& rGazeY);
}
