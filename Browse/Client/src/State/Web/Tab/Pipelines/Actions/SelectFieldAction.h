//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Action to choose option of select field on a webpage.
// - Input: none
// - Output: none

#ifndef SELECTFIELDACTION_H_
#define SELECTFIELDACTION_H_

#include "src/State/Web/Tab/Pipelines/Actions/Action.h"

class SelectFieldAction : public Action
{
public:

    // Constructor
	SelectFieldAction(TabInteractionInterface* pTab);

    // Destructor
    virtual ~SelectFieldAction();

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

#endif // SELECTFIELDACTION_H_
