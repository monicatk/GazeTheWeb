//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "PointingEvaluationPipeline.h"
#include "src/State/Web/Tab/Pipelines/Actions/ZoomCoordinateAction.h"
#include "src/State/Web/Tab/Pipelines/Actions/DriftCorrectionAction.h"
#include "src/State/Web/Tab/Pipelines/Actions/DynamicDriftCorrectionAction.h"
#include "src/State/Web/Tab/Pipelines/Actions/LeftMouseButtonClickAction.h"

PointingEvaluationPipeline::PointingEvaluationPipeline(TabInteractionInterface* pTab, PointingApproach approach) : Pipeline(pTab)
{
	// General pointer to pointing action
	std::shared_ptr<Action> spPointingAction = nullptr;

	// Decide by enumeration which approach to use
	switch (approach)
	{
	case PointingApproach::ZOOM:
	{
		spPointingAction = std::make_shared<ZoomCoordinateAction>(_pTab);
		break;
	}
	case PointingApproach::DRIFT_CORRECTION:
	{
		spPointingAction = std::make_shared<DriftCorrectionAction>(_pTab);
		break;
	}
	case PointingApproach::DYNAMIC_DRIFT_CORRECTION:
	{
		spPointingAction = std::make_shared<DynamicDriftCorrectionAction>(_pTab);
		break;
	}
	}

	// Push back pointing action
	_actions.push_back(spPointingAction);

	// Push back left mouse click
	std::shared_ptr<LeftMouseButtonClickAction> spMouseAction = std::make_shared<LeftMouseButtonClickAction>(_pTab);
	_actions.push_back(spMouseAction);

	// Connect actions (TODO: pointing is in CEFPixel space, mouse action in WebView space. Fix that!!!)
	std::unique_ptr<ActionConnector> upConnector =
		std::unique_ptr<ActionConnector>(new ActionConnector(spPointingAction, spMouseAction));
	upConnector->ConnectVec2("coordinate", "coordinate");
	_connectors.push_back(std::move(upConnector));
}
