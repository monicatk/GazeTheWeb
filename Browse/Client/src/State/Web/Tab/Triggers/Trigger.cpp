//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "Trigger.h"

Trigger::Trigger(TabInteractionInterface* pTab)
{
    _pTab = pTab;
}

Trigger::~Trigger()
{
    // Nothing to do
}
