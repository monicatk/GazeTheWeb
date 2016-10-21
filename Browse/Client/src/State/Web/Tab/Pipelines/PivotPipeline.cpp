//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "PivotPipeline.h"
#include "src/State/Web/Tab/Pipelines/Actions/ZoomCoordinateAction.h"
#include "src/State/Web/Tab/Pipelines/Actions/PivotMenuAction.h"

PivotPipeline::PivotPipeline(TabInteractionInterface* pTab) : Pipeline(pTab)
{
     // Add some actions
    std::unique_ptr<ZoomCoordinateAction> upZoomCoordinateAction =
        std::unique_ptr<ZoomCoordinateAction>(new ZoomCoordinateAction(_pTab));
    Action* pZoomCoordinateAction = upZoomCoordinateAction.get();
    _actions.push_back(std::move(upZoomCoordinateAction));

    std::unique_ptr<PivotMenuAction> upPivotMenuAction =
        std::unique_ptr<PivotMenuAction>(new PivotMenuAction(_pTab));
    Action* pPivotMenuAction = upPivotMenuAction.get();
    _actions.push_back(std::move(upPivotMenuAction));

    // Connect those actions
    std::unique_ptr<ActionConnector> upConnector =
        std::unique_ptr<ActionConnector>(new ActionConnector(pZoomCoordinateAction, pPivotMenuAction));
    upConnector->ConnectVec2("coordinate", "coordinate");
    _connectors.push_back(std::move(upConnector));
}
