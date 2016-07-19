//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Action to emulate left mouse button click.

// Notes
// - Input: vec2 coordinate
// - Output: none

#ifndef LEFTMOUSEBUTTONCLICKACTION_H_
#define LEFTMOUSEBUTTONCLICKACTION_H_

#include "Action.h"

class LeftMouseButtonClickAction : public Action
{
public:

    // Constructor
    LeftMouseButtonClickAction(TabInteractionInterface* pTab);

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

#endif // LEFTMOUSEBUTTONCLICKACTION_H_