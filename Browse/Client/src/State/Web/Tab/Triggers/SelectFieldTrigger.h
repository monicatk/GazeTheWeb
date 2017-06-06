//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Implementation of trigger generated from select fields.

#ifndef SELECTFIELDTRIGGER_H_
#define SELECTFIELDTRIGGER_H_

#include "src/State/Web/Tab/Triggers/DOMTrigger.h"

class SelectFieldTrigger : public DOMTrigger<DOMSelectField>
{
public:

	// Constructor
	SelectFieldTrigger(TabInteractionInterface* pTab, std::vector<Trigger*>& rTriggerCollection, std::shared_ptr<DOMSelectField> spNode);

	// Destructor
	virtual ~SelectFieldTrigger();

	// Update
	virtual bool Update(float tpf, const std::shared_ptr<const TabInput> spInput);
};

#endif // SELECTFIELDTRIGGER_H_