//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "TextInputPipeline.h"
#include "src/State/Web/Tab/Interface/TabInteractionInterface.h"
#include "src/State/Web/Tab/Interface/TabActionInterface.h"
#include "src/State/Web/Tab/Pipelines/Actions/KeyboardAction.h"
#include "src/State/Web/Tab/Pipelines/Actions/TextInputAction.h"
#include "src/State/Web/Tab/Pipelines/Actions/LeftMouseButtonClickAction.h"

TextInputPipeline::TextInputPipeline(TabInteractionInterface* pTab, std::shared_ptr<DOMNode> spNode) : Pipeline(pTab)
{
    // At first, click in text field
	std::shared_ptr<LeftMouseButtonClickAction> spLeftMouseButtonClickAction = std::make_shared<LeftMouseButtonClickAction>(_pTab);
	_actions.push_back(spLeftMouseButtonClickAction);

    // Then, do input via keyboard
	std::shared_ptr<KeyboardAction> spKeyboardAction = std::make_shared<KeyboardAction>(_pTab);
	_actions.push_back(spKeyboardAction);

    // At last, fill input into text field
	std::shared_ptr<TextInputAction> spTextInputAction = std::make_shared<TextInputAction>(_pTab);
	_actions.push_back(spTextInputAction);

    // Fill some values directly
    glm::vec2 clickCEFPixelCoordinates = spNode->GetCenter();
	double webViewPixelX = clickCEFPixelCoordinates.x;
	double webViewPixelY = clickCEFPixelCoordinates.y;
	_pTab->ConvertToWebViewPixel(webViewPixelX, webViewPixelY);
    spLeftMouseButtonClickAction->SetInputValue("coordinate", glm::vec2(webViewPixelX, webViewPixelY));
	spLeftMouseButtonClickAction->SetInputValue("visualize", 0);
    spTextInputAction->SetInputValue("frameId", spNode->GetFrameID());
    spTextInputAction->SetInputValue("nodeId", spNode->GetNodeID());

    // Connect those actions
    std::unique_ptr<ActionConnector> upConnector =
        std::unique_ptr<ActionConnector>(new ActionConnector(spKeyboardAction, spTextInputAction));
    upConnector->ConnectString16("text", "text");
    upConnector->ConnectInt("submit", "submit");
    _connectors.push_back(std::move(upConnector));
}
