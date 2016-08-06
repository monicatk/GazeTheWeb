//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "TextInputPipeline.h"
#include "src/State/Web/Tab/Interface/TabInteractionInterface.h"
#include "src/State/Web/Tab/Pipelines/Actions/KeyboardAction.h"
#include "src/State/Web/Tab/Pipelines/Actions/TextInputAction.h"
#include "src/State/Web/Tab/Pipelines/Actions/LeftMouseButtonClickAction.h"

TextInputPipeline::TextInputPipeline(TabInteractionInterface* pTab, std::shared_ptr<DOMNode> spNode) : Pipeline(pTab)
{
    // At first, click in text field
    std::unique_ptr<LeftMouseButtonClickAction> upLeftMouseButtonClickAction =
    std::unique_ptr<LeftMouseButtonClickAction>(new LeftMouseButtonClickAction(_pTab));
    Action* pLeftMouseButtonClickAction = upLeftMouseButtonClickAction.get();
    _actions.push_back(std::move(upLeftMouseButtonClickAction));

    // Then, do input via keyboard
    std::unique_ptr<KeyboardAction> upKeyboardAction =
        std::unique_ptr<KeyboardAction>(new KeyboardAction(_pTab));
    Action* pKeyboardAction = upKeyboardAction.get();
    _actions.push_back(std::move(upKeyboardAction));

    // At last, fill input into text field
    std::unique_ptr<TextInputAction> upTextInputAction =
        std::unique_ptr<TextInputAction>(new TextInputAction(_pTab));
    Action* pTextInputAction = upTextInputAction.get();
    _actions.push_back(std::move(upTextInputAction));

    // Fill some values directly
    int webViewWidth, webViewHeight;
    _pTab->GetWebViewTextureResolution(webViewWidth, webViewHeight);
    glm::vec2 clickCoordinates = spNode->GetCenter();
    clickCoordinates.x /= (float)webViewWidth; // to relative coordinates
    clickCoordinates.y /= (float)webViewHeight; // to relative coordinates
    pLeftMouseButtonClickAction->SetInputValue("coordinate", clickCoordinates);
	pLeftMouseButtonClickAction->SetInputValue("visualize", 0);
    pTextInputAction->SetInputValue("frameId", spNode->GetFrameID());
    pTextInputAction->SetInputValue("nodeId", spNode->GetNodeID());

    // Connect those actions
    std::unique_ptr<ActionConnector> upConnector =
        std::unique_ptr<ActionConnector>(new ActionConnector(pKeyboardAction, pTextInputAction));
    upConnector->ConnectString16("text", "text");
    upConnector->ConnectInt("submit", "submit");
    _connectors.push_back(std::move(upConnector));
}
