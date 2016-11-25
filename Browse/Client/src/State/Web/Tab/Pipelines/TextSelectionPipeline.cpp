//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "TextSelectionPipeline.h"
#include "src/State/Web/Tab/Pipelines/Actions/TextSelectionAction.h"
#include "src/State/Web/Tab/Pipelines/Actions/ZoomCoordinateAction.h"
#include "src/State/Web/Tab/Pipelines/Actions/HintAction.h"
#include "src/Utils/MakeUnique.h"

TextSelectionPipeline::TextSelectionPipeline(TabInteractionInterface* pTab) : Pipeline(pTab)
{
    // Add hint action
    auto spStartHintAction = std::make_shared<HintAction>(_pTab, "hint:start_selection", "start_selection");
    _actions.push_back(spStartHintAction);

    // Add zoom coordinate to determine start of text selection
    auto spZoomCoordinateAction = std::make_shared<ZoomCoordinateAction>(_pTab, false);
    _actions.push_back(spZoomCoordinateAction);

    // Add hint action
    auto spEndHintAction = std::make_shared<HintAction>(_pTab, "hint:end_selection", "end_selection");
    _actions.push_back(spEndHintAction);

	// Add text selection action
    auto spTextSelectionAction = std::make_shared<TextSelectionAction>(_pTab);
    _actions.push_back(spTextSelectionAction);

    // Connect actions
    auto upConnector =
        std::make_unique<ActionConnector>(spZoomCoordinateAction, spTextSelectionAction);
    upConnector->ConnectVec2("coordinate", "coordinate");
    _connectors.push_back(std::move(upConnector));
}
