//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "Trigger.h"
#include <algorithm>

Trigger::Trigger(TabInteractionInterface* pTab, std::vector<Trigger*>& rTriggerCollection)
{
	// Store pointers
    _pTab = pTab;
	_pTriggerCollection = &rTriggerCollection;

	// Register itself to trigger collection
	_pTriggerCollection->push_back(this);
}

Trigger::~Trigger()
{
    // Unregister from trigger collection
	_pTriggerCollection->erase(
		std::remove(
			_pTriggerCollection->begin(),
			_pTriggerCollection->end(),
			this),
		_pTriggerCollection->end());
}
