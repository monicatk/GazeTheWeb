//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Action to input text into a text input field on a webpage.
// - Input: int64 frameId
// - Input: int nodeId
// - Input: std::u16string text
// - Input: int submit (0 if not, else submit)
// - Output: none

#ifndef TEXTINPUTACTION_H_
#define TEXTINPUTACTION_H_

#include "src/State/Web/Tab/Pipelines/Actions/Action.h"

class TextInputAction : public Action
{
public:

    // Constructor
    TextInputAction(TabInteractionInterface* pTab);

    // Destructor
    virtual ~TextInputAction();

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

#endif // TEXTINPUTACTION_H_
