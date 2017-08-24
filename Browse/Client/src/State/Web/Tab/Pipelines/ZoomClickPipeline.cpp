//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "ZoomClickPipeline.h"
#include "src/State/Web/Tab/Pipelines/Actions/CoordinateActions/DriftCorrectionAction.h"
#include "src/State/Web/Tab/Pipelines/Actions/LinkNavigationAction.h"

ZoomClickPipeline::ZoomClickPipeline(TabInteractionInterface* pTab) : Pipeline(pTab)
{
    // Push back zoom coordinate with drift correction action
	std::shared_ptr<DriftCorrectionAction> spDriftCorrectionAction = std::make_shared<DriftCorrectionAction>(_pTab);
	_actions.push_back(spDriftCorrectionAction);

	// Push back link navigation action
	std::shared_ptr<LinkNavigationAction> spLinkNavigationAction = std::make_shared<LinkNavigationAction>(_pTab);
	_actions.push_back(spLinkNavigationAction);

    // Connect actions
    std::unique_ptr<ActionConnector> upConnector =
        std::unique_ptr<ActionConnector>(new ActionConnector(spDriftCorrectionAction, spLinkNavigationAction));
    upConnector->ConnectVec2("coordinate", "coordinate");
    _connectors.push_back(std::move(upConnector));
}
