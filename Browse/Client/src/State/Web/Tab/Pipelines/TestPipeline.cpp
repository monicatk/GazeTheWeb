//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "TestPipeline.h"
#include "src/State/Web/Tab/Pipelines/Actions/ZoomCoordinateAction.h"
#include "src/State/Web/Tab/Pipelines/Actions/LeftMouseButtonClickAction.h"

TestPipeline::TestPipeline(TabInteractionInterface* pTab) : Pipeline(pTab)
{
    // Add some actions
    std::unique_ptr<ZoomCoordinateAction> upZoomCoordinateAction =
        std::unique_ptr<ZoomCoordinateAction>(new ZoomCoordinateAction(_pTab));
    Action* pZoomCoordinateAction = upZoomCoordinateAction.get();
    _actions.push_back(std::move(upZoomCoordinateAction));

    std::unique_ptr<LeftMouseButtonClickAction> upLeftMouseButtonClickAction =
        std::unique_ptr<LeftMouseButtonClickAction>(new LeftMouseButtonClickAction(_pTab));
    Action* pLeftMouseButtonClickAction = upLeftMouseButtonClickAction.get();
    _actions.push_back(std::move(upLeftMouseButtonClickAction));

    // Connect those actions
    std::unique_ptr<ActionConnector> upConnector =
        std::unique_ptr<ActionConnector>(new ActionConnector(pZoomCoordinateAction, pLeftMouseButtonClickAction));
    upConnector->ConnectVec2("coordinate", "coordinate");
    _connectors.push_back(std::move(upConnector));
}
