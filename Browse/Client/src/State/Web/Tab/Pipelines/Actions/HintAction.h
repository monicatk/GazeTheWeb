//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Action for showing a hint.

#ifndef HINTACTION_H_
#define HINTACTION_H_

#include "src/State/Web/Tab/Pipelines/Actions/Action.h"
#include "src/Utils/LerpValue.h"

class HintAction : public Action
{
public:

    // Constructor, takes key to localization string and id for unique layout id
    HintAction(TabInteractionInterface* pTab, std::string key, std::string id);

    // Desctructor
    virtual ~HintAction();

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

    // Index of floating frame showing hint
    int _overlayFrameIndex = -1;

    // Id of button in overlay
    std::string _overlayButtonId;

    // Bool which indicates whether input is complete
    bool _done = false;
};

#endif // HINTACTION_H_
