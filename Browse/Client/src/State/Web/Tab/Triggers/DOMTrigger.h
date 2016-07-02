//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Implementation of trigger generated from DOM elements.

#ifndef DOMTRIGGER_H_
#define DOMTRIGGER_H_

#include "src/State/Web/Tab/Triggers/Trigger.h"
#include "src/State/Web/Tab/DOMNode.h"
#include "submodules/eyeGUI/include/eyeGUI.h"
#include <memory>

class DOMTrigger : public Trigger
{
public:

    // Constructor
    DOMTrigger(TabInteractionInterface* pTab, std::shared_ptr<DOMNode> spNode);

    // Destructor
    virtual ~DOMTrigger();

    // Update
    virtual bool Update(float tpf, TabInput& rTabInput);

    // Draw
    virtual void Draw() const;

    // Activate
    virtual void Activate();

    // Deactivate
    virtual void Deactivate();

private:

    // Calculate position of overlay button
    void CalculatePositionOfOverlayButton(float& rRelativePositionX, float& rRelativePositionY) const;

    // Shared pointer to node
    std::shared_ptr<DOMNode> _spNode;

    // Index of floating frame in Tab's overlay
    int _overlayFrameIndex = -1;

    // Id of button in overlay
    std::string _overlayButtonId;

    // Size of overlay button
    float _size = 0.1f;

    // Bool to remember that it was triggered
    bool _triggered = false;
};

#endif // TRIGGER_H_
