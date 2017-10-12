//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#ifndef EYETRACKERGEOMETRY_H_
#define EYETRACKERGEOMETRY_H_

// Struct about geometry of screen and eyetracker
struct EyetrackerGeometry
{
	int monitorWidth; // millimeter
	int monitorHeight; // millimeter
	int mountingAngle; // degree
	int relativeDistanceHeight; // millimeter distance between eyetracker and monitor in height
	int relativeDistanceDepth; // millimeter distance between eyetracker and monitor in depth
};

#endif EYETRACKERGEOMETRY_H_