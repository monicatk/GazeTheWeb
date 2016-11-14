//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "KeyboardAction.h"
#include "src/State/Web/Tab/Interface/TabInteractionInterface.h"

KeyboardAction::KeyboardAction(TabInteractionInterface *pTab) : Action(pTab)
{
    // Add in- and output data slots
    AddString16OutputSlot("text");
    AddIntOutputSlot("submit");

    // Create id, which is unique in overlay
    _overlayKeyboardId = "text_input_action_keyboard";
    _overlayCompleteButtonId = "text_input_action_complete_button";
    _overlaySubmitButtonId = "text_input_action_submit_button";
    _overlayDeleteCharacterButtonId = "text_input_action_delete_character_button";
    _overlaySpaceButtonId = "text_input_action_space_button";
	_overlayTextEditId = "text_input_action_text_edit";
    _overlayWordSuggestId = "text_input_action_word_suggest";
	_overlayShiftButtonId = "text_input_action_shift_button";

    // Id mapper for brick to change ids from file to the used ones
    std::map<std::string, std::string> idMapper;
    idMapper.emplace("keyboard", _overlayKeyboardId);
    idMapper.emplace("complete", _overlayCompleteButtonId);
    idMapper.emplace("submit", _overlaySubmitButtonId);
    idMapper.emplace("delete_character", _overlayDeleteCharacterButtonId);
    idMapper.emplace("space", _overlaySpaceButtonId);
    idMapper.emplace("text_edit", _overlayTextEditId);
    idMapper.emplace("word_suggest", _overlayWordSuggestId);
	idMapper.emplace("shift", _overlayShiftButtonId);

    // Calculate size of overlay
    float x, y, sizeX, sizeY;
    x = (float)_pTab->GetWebViewX() / (float)_pTab->GetWindowWidth();
	y = (float)_pTab->GetWebViewY() / (float)_pTab->GetWindowHeight();
    sizeX = (float)_pTab->GetWebViewWidth() / (float)_pTab->GetWindowWidth();
    sizeY = (float)_pTab->GetWebViewHeight() / (float)_pTab->GetWindowHeight();

    // Add overlay
    _overlayFrameIndex = _pTab->AddFloatingFrameToOverlay("bricks/actions/Keyboard.beyegui", x, y, sizeX, sizeY, idMapper);

    // Register listeners

	// Keyboard
    _pTab->RegisterKeyboardListenerInOverlay(
        _overlayKeyboardId,
        [&](std::u16string value)
        {
			// Add content from keyboard
			_pTab->AddContentAtCursorInTextEdit(_overlayTextEditId, value);

			// Refresh suggestions
            _pTab->DisplaySuggestionsInWordSuggest(_overlayWordSuggestId, _pTab->GetActiveEntityContentInTextEdit(_overlayTextEditId));
        });

	// Complete button
    _pTab->RegisterButtonListenerInOverlay(
        _overlayCompleteButtonId,
        [&]() // down callback
        {
            this->_complete = true;
        },
		[](){}); // up callback

	// Submit button
    _pTab->RegisterButtonListenerInOverlay(
        _overlaySubmitButtonId,
        [&]() // down callback
        {
            this->_submit = true;
            this->_complete = true;
        },
		[](){}); // up callback

	// Delete character button
    _pTab->RegisterButtonListenerInOverlay(
        _overlayDeleteCharacterButtonId,
        [&]() // down callback
        {
			// Delete an letter from content
			_pTab->DeleteContentAtCursorInTextEdit(_overlayTextEditId, -1);

			// Refresh suggestions
			_pTab->DisplaySuggestionsInWordSuggest(_overlayWordSuggestId, _pTab->GetActiveEntityContentInTextEdit(_overlayTextEditId));
        },
		[](){}); // up callback

	// Space button
    _pTab->RegisterButtonListenerInOverlay(
        _overlaySpaceButtonId,
        [&]() // down callback
        {
			// Add space to content
			_pTab->AddContentAtCursorInTextEdit(_overlayTextEditId, u" ");

			// Clear suggestions
			_pTab->DisplaySuggestionsInWordSuggest(_overlayWordSuggestId, u"");
        },
		[](){}); // up callback

	// Word suggestion
    _pTab->RegisterWordSuggestListenerInOverlay(
        _overlayWordSuggestId,
        [&](std::u16string value)
        {
			// Fill chosen suggestion into text edit
			_pTab->SetActiveEntityContentInTextEdit(_overlayTextEditId, value);
        });

	// Shift button (switch)
	_pTab->RegisterButtonListenerInOverlay(
		_overlayShiftButtonId,
		[&]() // down callback
		{
			this->_pTab->SetCaseOfKeyboardLetters(_overlayKeyboardId, true);
		},
		[&]() // up callback
		{
			this->_pTab->SetCaseOfKeyboardLetters(_overlayKeyboardId, false);
		});
}

KeyboardAction::~KeyboardAction()
{
    // Delete overlay frame
    _pTab->RemoveFloatingFrameFromOverlay(_overlayFrameIndex);

    // Unregister listener of elements in overlay
    _pTab->UnregisterKeyboardListenerInOverlay(_overlayKeyboardId);
    _pTab->UnregisterButtonListenerInOverlay(_overlayCompleteButtonId);
    _pTab->UnregisterButtonListenerInOverlay(_overlaySubmitButtonId);
    _pTab->UnregisterButtonListenerInOverlay(_overlayDeleteCharacterButtonId);
    _pTab->UnregisterButtonListenerInOverlay(_overlaySpaceButtonId);
    _pTab->UnregisterWordSuggestListenerInOverlay(_overlayWordSuggestId);
	_pTab->UnregisterButtonListenerInOverlay(_overlayShiftButtonId);
}

bool KeyboardAction::Update(float tpf, TabInput tabInput)
{
    if (_complete)
    {
        // Fill collected input to output
        SetOutputValue("text", _pTab->GetContentOfTextEdit(_overlayTextEditId));

        // Submit text directly if wished
        SetOutputValue("submit", _submit);

        // Action is now finished
        return true;
    }
    else
    {
        return false;
    }
}

void KeyboardAction::Draw() const
{
    // Nothing to do
}

void KeyboardAction::Activate()
{
    _pTab->SetVisibilyOfFloatingFrameInOverlay(_overlayFrameIndex, true);
}

void KeyboardAction::Deactivate()
{
    _pTab->SetVisibilyOfFloatingFrameInOverlay(_overlayFrameIndex, false);
}

void KeyboardAction::Abort()
{
    // Nothing to do
}