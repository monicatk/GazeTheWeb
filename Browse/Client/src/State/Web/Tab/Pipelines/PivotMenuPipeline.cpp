//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "PivotMenuPipeline.h"
#include "src/State/Web/Tab/Pipelines/Actions/ZoomCoordinateAction.h"
#include "src/State/Web/Tab/Pipelines/Actions/PivotMenuAction.h"

PivotMenuPipeline::PivotMenuPipeline(TabInteractionInterface* pTab) : Pipeline(pTab)
{
	// Push back zoom coordinate action
	std::shared_ptr<ZoomCoordinateAction> spZoomCoordinateAction = std::make_shared<ZoomCoordinateAction>(_pTab);
    _actions.push_back(spZoomCoordinateAction);

	// Push back pivot menu action
	std::shared_ptr<PivotMenuAction> spPivotMenuAction = std::make_shared<PivotMenuAction>(_pTab);
	_actions.push_back(spPivotMenuAction);

    // Connect actions
    std::unique_ptr<ActionConnector> upConnector =
        std::unique_ptr<ActionConnector>(new ActionConnector(spZoomCoordinateAction, spPivotMenuAction));
    upConnector->ConnectVec2("coordinate", "coordinate");
    _connectors.push_back(std::move(upConnector));
}
