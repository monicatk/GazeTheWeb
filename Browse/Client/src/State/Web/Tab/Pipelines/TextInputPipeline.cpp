//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "TextInputPipeline.h"
#include "src/State/Web/Tab/Interface/TabInteractionInterface.h"
#include "src/State/Web/Tab/Interface/TabActionInterface.h"
#include "src/State/Web/Tab/Pipelines/Actions/KeyboardAction.h"
#include "src/State/Web/Tab/Pipelines/Actions/TextInputAction.h"
#include "submodules/eyeGUI/include/eyeGUI.h"

TextInputPipeline::TextInputPipeline(TabInteractionInterface* pTab, std::shared_ptr<DOMTextInput> spNode) : Pipeline(pTab)
{
	// Get current text from node
	std::string text = spNode->GetText();
	std::u16string text16;
	eyegui_helper::convertUTF8ToUTF16(text, text16);

    // Then, do input via keyboard
	std::shared_ptr<KeyboardAction> spKeyboardAction = std::make_shared<KeyboardAction>(_pTab);
	spKeyboardAction->SetInputValue<std::u16string>("text", text16);
	_actions.push_back(spKeyboardAction);

    // At last, fill input into text field
	std::shared_ptr<TextInputAction> spTextInputAction = std::make_shared<TextInputAction>(_pTab);
	_actions.push_back(spTextInputAction);

    // Fill some values directly
	if (spNode->GetRects().size() > 0)
	{
		const auto& rect = spNode->GetRects()[0];
		glm::vec2 clickCEFPixelCoordinates = rect.Center();
		double webViewPixelX = clickCEFPixelCoordinates.x;
		double webViewPixelY = clickCEFPixelCoordinates.y;
		_pTab->ConvertToWebViewPixel(webViewPixelX, webViewPixelY);
	}

    //spTextInputAction->SetInputValue("frameId", spNode->GetFrameID());	// DEPRECATED
    spTextInputAction->SetInputValue("nodeId", spNode->GetId());

    // Connect those actions
    std::unique_ptr<ActionConnector> upConnector =
        std::unique_ptr<ActionConnector>(new ActionConnector(spKeyboardAction, spTextInputAction));
    upConnector->ConnectString16("text", "text");
    upConnector->ConnectInt("submit", "submit");
    _connectors.push_back(std::move(upConnector));
}
