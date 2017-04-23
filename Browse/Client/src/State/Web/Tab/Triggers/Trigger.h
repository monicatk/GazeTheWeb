//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Abstract superclass for all kind of triggers.

#ifndef TRIGGER_H_
#define TRIGGER_H_

#include "src/Utils/TabInput.h"
#include "src/State/Web/Tab/Interface/TabInteractionInterface.h"
#include <vector>

class Trigger
{
public:

    // Constructor which registers itself in trigger collection
    Trigger(TabInteractionInterface* pTab, std::vector<Trigger*>& rTriggerCollection);

    // Destructor, unregisters itself from trigger collection
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
    TabInteractionInterface* _pTab = nullptr;

	// Keep reference to trigger collection
	std::vector<Trigger*>* _pTriggerCollection = nullptr;
};

#endif // TRIGGER_H_
