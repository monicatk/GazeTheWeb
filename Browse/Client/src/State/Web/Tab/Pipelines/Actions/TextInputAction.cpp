//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "TextInputAction.h"
#include "src/State/Web/Tab/Interface/TabInteractionInterface.h"
#include "submodules/eyeGUI/include/eyeGUI.h"

TextInputAction::TextInputAction(TabInteractionInterface* pTab, std::shared_ptr<DOMTextInputInteraction> spInteractionNode) :
	Action(pTab),
	_spInteractionNode(spInteractionNode)
{
    // Add in- and output data slots
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
    std::u16string text;
    int submit = 0;
    GetInputValue("text", text);
    GetInputValue("submit", submit);

	// Convert u16string to string
	std::string text8;
	eyegui_helper::convertUTF16ToUTF8(text, text8);

	// Input text
	_spInteractionNode->InputText(text8, submit > 0); // TODO: Call LSL Logging?


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
