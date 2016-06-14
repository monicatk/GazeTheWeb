//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "Eyetracker.h"
#include "src/Input/Eyetracker/EyetrackerGlobal.h"
#include "src/Global.h"
#include <algorithm>

Eyetracker::~Eyetracker()
{
    // Disconnect does not work here because specialDisconnect is virtual
}

bool Eyetracker::Connect()
{
    // Remember about connection
    if (!_connected)
    {
        _connected = SpecialConnect();
    }
    return _connected;
}

bool Eyetracker::Disconnect()
{
    // Check whether necessary
    if (_connected)
    {
        _connected = false;
		return SpecialDisconnect();
    }
	else
	{
		return false;
	}
}

void Eyetracker::Update(float tpf)
{
	// Collect k or less valid samples
	std::vector<double> gazeXSamples, gazeYSamples;
	eyetracker_global::GetKOrLessValidRawGazeEntries(EYETRACKER_AVERAGE_SAMPLE_COUNT, gazeXSamples, gazeYSamples);

	// Average the given samples
	double sum = 0;
	for (double x : gazeXSamples)
	{
		sum += x;
	}
	_gazeX = sum / gazeXSamples.size();
	sum = 0;
	for (double y : gazeYSamples)
	{
		sum += y;
	}
	_gazeY = sum / gazeYSamples.size();
}

double Eyetracker::GetGazeX() const
{
    return _gazeX;
}

double Eyetracker::GetGazeY() const
{
    return _gazeY;
}
