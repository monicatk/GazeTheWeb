//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "Action.h"

Action::Action(TabInteractionInterface* pTab)
{
    // Save pointer to Tab interface
    _pTab = pTab;
}

Action::~Action()
{
    // Implemented completely virtual destructor
}
