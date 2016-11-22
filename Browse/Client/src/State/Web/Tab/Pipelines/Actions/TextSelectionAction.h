//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Action for text selection.

#ifndef TEXTSELECTIONACTION_H_
#define TEXTSELECTIONACTION_H_

#include "Action.h"
#include "src/Utils/LerpValue.h"

class TextSelectionAction : public Action
{
public:

    // Constructor
	TextSelectionAction(TabInteractionInterface* pTab);

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

};

#endif // TEXTSELECTIONACTION_H_