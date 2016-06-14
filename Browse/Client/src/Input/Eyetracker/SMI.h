//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Eyetracker implementation for SMI Red.

#ifdef SMI_SUPPORT
#ifndef SMI_H_
#define SMI_H_

#include "src/Input/Eyetracker/Eyetracker.h"
#include "externals/iViewX/include/iViewXAPI.h"

class SMI : public Eyetracker
{
protected:

    // Special connect
    virtual bool SpecialConnect();

    // Special disconnect
    virtual bool SpecialDisconnect();
};

#endif // SMI_H_
#endif // SMI_SUPPORT
