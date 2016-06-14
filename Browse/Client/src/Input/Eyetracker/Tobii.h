//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Eyetracker implementation for Tobii EyeX.

#ifdef TOBII_SUPPORT
#ifndef TOBII_H_
#define TOBII_H_

#include "src/Input/Eyetracker/Eyetracker.h"
#include "externals/TobiiEyeXSDK/include/eyex/EyeX.h"

class Tobii : public Eyetracker
{
protected:

    // Special connect
    virtual bool SpecialConnect();

    // Special disconnect
    virtual bool SpecialDisconnect();
};

#endif // TOBII_H_
#endif // TOBII_SUPPORT
