//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#ifndef EYETRACKERINFO_H_
#define EYETRACKERINFO_H_

// Struct of info
struct EyetrackerInfo
{
	// Fields
	bool connected = false;
	int samplerate = -1;
	bool geometrySetupSuccessful = false;
};

#endif EYETRACKERINFO_H_