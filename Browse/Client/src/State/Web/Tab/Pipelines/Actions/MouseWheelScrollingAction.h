//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Action to emulate mouse wheel scrolling.

// Notes
// - Input: vec2 scrolling
// - Output: none

#ifndef MOUSEWHEELSCROLLINGACTION_H_
#define MOUSEWHEELSCROLLINGACTION_H_

#include "Action.h"

class MouseWheelScrollingAction : public Action
{
public:

    // Constructor
    MouseWheelScrollingAction(TabInteractionInterface* pTab);

    // Update retuns whether finished with execution
    virtual bool Update(float tpf, TabInput tabInput);

    // Draw
    virtual void Draw() const;

    // Activate
    virtual void Activate();

    // Deactivate
    virtual void Deactivate();

    // Abort
    virtual void Abort();
};

#endif // MOUSEWHEELSCROLLINGACTION_H_
