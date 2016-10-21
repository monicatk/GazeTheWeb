//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Action for displaying pivot menu.
// - Input: vec2 coordinate in pixels
// - Output: none

#ifndef PIVOTMENUACTION_H_
#define PIVOTMENUACTION_H_

#include "Action.h"
#include "src/Utils/LerpValue.h"

class PivotMenuAction : public Action
{
public:

    // Constructor
    PivotMenuAction(TabInteractionInterface* pTab);

    // Destructor
    ~PivotMenuAction();

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

protected:

    // Index of floating frame for menu in Tab's overlay
    int _menuFrameIndex = -1;

};

#endif // PIVOTMENUACTION_H_
