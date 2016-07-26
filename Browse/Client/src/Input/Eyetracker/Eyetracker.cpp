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

void Eyetracker::Update(
	float tpf,
	int windowX,
	int windowY,
	int windowWidth,
	int windowHeight)
{
	// Collect k or less valid samples
	std::vector<double> gazeXSamples, gazeYSamples;
	eyetracker_global::GetKOrLessValidRawGazeEntries(EYETRACKER_AVERAGE_SAMPLE_COUNT, gazeXSamples, gazeYSamples);

	// TODO: maybe use something like queue to collect raw data, clamp it and then add it to a buffer. Then
	// every sample is clamped with the correct (or at least more corresponding) window coordinates. At the moment,
	// All collected samples are clamped with the current window coordinates

	// Convert parameters to double
	double windowXDouble = (double)windowX;
	double windowYDouble = (double)windowY;
	double windowWidthDouble = (double)windowWidth;
	double windowHeightDouble = (double)windowHeight;

	// Average the given samples
	double sum = 0;
	for (double x : gazeXSamples)
	{
		// Do some clamping according to window coordinates
		double clampedX = x - windowXDouble;
		clampedX = clampedX > 0.0 ? clampedX : 0.0;
		clampedX = clampedX < windowWidthDouble ? clampedX : windowWidthDouble;
		sum += clampedX;
	}
	_gazeX = sum / gazeXSamples.size();
	sum = 0;
	for (double y : gazeYSamples)
	{
		// Do some clamping according to window coordinates
		double clampedY = y - windowYDouble;
		clampedY = clampedY > 0.0 ? clampedY : 0.0;
		clampedY = clampedY < windowHeightDouble ? clampedY : windowHeightDouble;
		sum += clampedY;
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
