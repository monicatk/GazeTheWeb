//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "SelectFieldTrigger.h"
#include "src/State/Web/Tab/Pipelines/SelectFieldPipeline.h"
#include "src/Singletons/LabStreamMailer.h"

SelectFieldTrigger::SelectFieldTrigger(TabInteractionInterface* pTab, std::vector<Trigger*>& rTriggerCollection, std::shared_ptr<DOMSelectField> spNode) : DOMTrigger<DOMSelectField>(pTab, rTriggerCollection, spNode, "bricks/triggers/SelectField.beyegui", "select_field")
{
	// Nothing to do here
}

SelectFieldTrigger::~SelectFieldTrigger()
{
	// Nothing to do here
}

bool SelectFieldTrigger::Update(float tpf, const std::shared_ptr<const TabInput> spInput)
{
	// Call super method
	bool triggered = DOMTrigger::Update(tpf, spInput);

	// When triggered, push back pipeline to input text
	if (triggered)
	{
		LabStreamMailer::instance().Send("Select field hit");

		_pTab->PushBackPipeline(
			std::move(
				std::unique_ptr<SelectFieldPipeline>(
					new SelectFieldPipeline(
						_pTab,
						_spNode))));
	}

	// Return whether triggered
	return triggered;
}