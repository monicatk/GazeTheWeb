//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "plugins/Eyetracker/Interface/Eyetracker.h"
#include "plugins/Eyetracker/Common/EyetrackerData.h"

bool Connect()
{
	return false;
}

bool Disconnect()
{
	return false;
}

void GetKOrLessValidRawGazeEntries(int k, std::vector<double>& rGazeX, std::vector<double>& rGazeY)
{
	eyetracker_global::GetKOrLessValidRawGazeEntries(k, rGazeX, rGazeY);
}