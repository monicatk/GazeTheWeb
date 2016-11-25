//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "TextSelectionPipeline.h"
#include "src/State/Web/Tab/Pipelines/Actions/TextSelectionAction.h"
#include "src/State/Web/Tab/Pipelines/Actions/ZoomCoordinateAction.h"
#include "src/Utils/MakeUnique.h"

TextSelectionPipeline::TextSelectionPipeline(TabInteractionInterface* pTab) : Pipeline(pTab)
{
    // Add zoom coordinate to determine start of text selection
    std::shared_ptr<ZoomCoordinateAction> spZoomCoordinateAction = std::make_shared<ZoomCoordinateAction>(_pTab, false);
    _actions.push_back(spZoomCoordinateAction);

	// Add text selection action
    auto spTextSelectionAction = std::make_shared<TextSelectionAction>(_pTab);
    _actions.push_back(spTextSelectionAction);

    // Connect actions
    std::unique_ptr<ActionConnector> upConnector =
        std::make_unique<ActionConnector>(spZoomCoordinateAction, spTextSelectionAction);
    upConnector->ConnectVec2("coordinate", "coordinate");
    _connectors.push_back(std::move(upConnector));
}
