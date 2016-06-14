//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "State.h"

State::State(Master* pMaster)
{
    _pMaster = pMaster;
}

State::~State()
{
    // Just implemented for the sake of C++
}

void State::Activate()
{
    _active = true;
}

void State::Deactivate()
{
    _active = false;
}

