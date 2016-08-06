//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Abstract supercall for all kind of triggers.

#ifndef TRIGGER_H_
#define TRIGGER_H_

#include "src/Utils/TabInput.h"
#include "src/State/Web/Tab/Interface/TabInteractionInterface.h"

class Trigger
{
public:

    // Constructor
    Trigger(TabInteractionInterface* pTab);

    // Destructor
    virtual ~Trigger() = 0;

    // Update, returns true if triggered
    virtual bool Update(float tpf, TabInput& rTabInput) = 0;

    // Draw
    virtual void Draw() const = 0;

    // Activate
    virtual void Activate() = 0;

    // Deactivate
    virtual void Deactivate() = 0;

protected:

    // Pointer to tab interface
    TabInteractionInterface* _pTab;
};

#endif // TRIGGER_H_
