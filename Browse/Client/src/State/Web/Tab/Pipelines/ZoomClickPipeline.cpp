//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "ZoomClickPipeline.h"
#include "src/State/Web/Tab/Pipelines/Actions/CoordinateActions/ZoomCoordinateAction.h"
#include "src/State/Web/Tab/Pipelines/Actions/LinkNavigationAction.h"

ZoomClickPipeline::ZoomClickPipeline(TabInteractionInterface* pTab) : Pipeline(pTab)
{
    // Push back zoom coordinate action
	std::shared_ptr<ZoomCoordinateAction> spZoomCoordinateAction = std::make_shared<ZoomCoordinateAction>(_pTab);
	_actions.push_back(spZoomCoordinateAction);

	// Push back link navigation action
	std::shared_ptr<LinkNavigationAction> spLinkNavigationAction = std::make_shared<LinkNavigationAction>(_pTab);
	_actions.push_back(spLinkNavigationAction);

    // Connect actions
    std::unique_ptr<ActionConnector> upConnector =
        std::unique_ptr<ActionConnector>(new ActionConnector(spZoomCoordinateAction, spLinkNavigationAction));
    upConnector->ConnectVec2("coordinate", "coordinate");
    _connectors.push_back(std::move(upConnector));
}
