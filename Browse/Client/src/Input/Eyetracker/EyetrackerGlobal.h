//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Global functions which are declared here but defined in a cpp. Filled by
// Eyetracker implementations and used by abstract superclass.

#include <vector>

namespace eyetracker_global
{
	void PushBackRawData(double gazeX, double gazeY, bool valid);
	void GetKOrLessValidRawGazeEntries(int k, std::vector<double>& rGazeX, std::vector<double>& rGazeY);
}
