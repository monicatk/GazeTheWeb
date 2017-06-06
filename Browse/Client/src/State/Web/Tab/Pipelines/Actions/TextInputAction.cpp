//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "TextInputAction.h"
#include "src/State/Web/Tab/Interface/TabInteractionInterface.h"
#include "submodules/eyeGUI/include/eyeGUI.h"

TextInputAction::TextInputAction(TabInteractionInterface *pTab) : Action(pTab)
{
    // Add in- and output data slots
    AddInt64InputSlot("frameId");
    AddIntInputSlot("nodeId");
    AddString16InputSlot("text");
    AddIntInputSlot("submit");
}

TextInputAction::~TextInputAction()
{
    // Nothing to do
}

bool TextInputAction::Update(float tpf, const std::shared_ptr<const TabInput> spInput)
{
    // Fetch input values
    int64 frameId = 0;
    int nodeId = 0;
    std::u16string text;
    int submit = 0;
    GetInputValue("frameId", frameId);
    GetInputValue("nodeId", nodeId);
    GetInputValue("text", text);
    GetInputValue("submit", submit);

	// Convert u16string to string
	std::string text8;
	eyegui_helper::convertUTF16ToUTF8(text, text8);

    // FORMERLY: Just pipe values to CEF mediator through Tab interface
	// NOW: Call InputText method on DOMNode object
    _pTab->InputTextData(frameId, nodeId, text8, submit != 0);

    // Action is done
    return true;
}

void TextInputAction::Draw() const
{
    // Nothing to do
}

void TextInputAction::Activate()
{
    // Nothing to do
}

void TextInputAction::Deactivate()
{
    // Nothing to do
}

void TextInputAction::Abort()
{
    // Nothing to do
}
