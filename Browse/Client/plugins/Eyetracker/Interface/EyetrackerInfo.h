//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#ifndef EYETRACKERINFO_H_
#define EYETRACKERINFO_H_

#include <vector>

// Struct of info
struct EyetrackerInfo
{
	// Fields
	bool connected = false;
	int samplerate = -1;
	bool geometrySetupSuccessful = false;
};

// Enumeration about calibration
enum CalibrationResult { CALIBRATION_NOT_SUPPORTED, CALIBRATION_OK, CALIBRATION_BAD, CALIBRATION_FAILED };

// Enumeration about calibration point
enum CalibrationPointResult { CALIBRATION_POINT_OK, CALIBRATION_POINT_BAD, CALIBRATION_POINT_FAILED };

// Struct of calibration point
struct CalibrationPoint
{
	CalibrationPoint(int positionX, int positionY, CalibrationPointResult result) : positionX(positionX), positionY(positionY), result(result) {};
	int positionX = 0;
	int positionY = 0;
	CalibrationPointResult result = CALIBRATION_POINT_FAILED;
};

// Vector with calibration info
typedef std::shared_ptr<std::vector<CalibrationPoint> > CalibrationInfo;

#endif EYETRACKERINFO_H_