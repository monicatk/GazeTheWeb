//============================================================================
// Distributed under the MIT License.
// Author: Raphael Menges (https://github.com/raphaelmenges)
//============================================================================

#include "TextInput.h"

// ### KEYBOARD LISTENER ###

InputListener::InputListener(TextInput* pTextInput)
{
	mpTextInput = pTextInput;
}

void InputListener::keyPressed(eyegui::Layout* pLayout, std::string id, std::string value)
{
	// Append string from keyboard
	mpTextInput->appendTextToCurrentWord(value);
}

// ### SUGGESTION LISTENER ###

SuggestionListener::SuggestionListener(TextInput* pTextInput)
{
	mpTextInput = pTextInput;
}

void SuggestionListener::chosen(eyegui::Layout* pLayout, std::string id, std::string value)
{
	mpTextInput->setCurrentWord(value);
}

// ### MULTIPLE BUTTON LISTENER ###

MultipleButtonsListener::MultipleButtonsListener(TextInput* pTextInput)
{
	mpTextInput = pTextInput;
}

void MultipleButtonsListener::hit(eyegui::Layout* pLayout, std::string id)
{
	// Nothing to do
}

void MultipleButtonsListener::down(eyegui::Layout* pLayout, std::string id)
{
	if (id == "space")
	{
		mpTextInput->nextWord();
	}
	else if (id == "finish")
	{
		mpTextInput->callback();
	}
}

void MultipleButtonsListener::up(eyegui::Layout* pLayout, std::string id)
{
	// Nothing to do
}

// ### TEXT INPUT ###

TextInput::TextInput(eyegui::GUI* pGUI, void(*textInputCallback) (std::string))
{
	// Save members
	mpGUI = pGUI;
	mTextInputCallback = textInputCallback;

	// Create layout
	mpLayout = eyegui::addLayout(mpGUI, "layouts/TextInput.xeyegui", false);

	// Listener
	mspInputListener = std::shared_ptr<InputListener>(new InputListener(this));
	eyegui::registerKeyboardListener(mpLayout, "keyboard", mspInputListener);

	mspSuggestionListener = std::shared_ptr<SuggestionListener>(new SuggestionListener(this));
	eyegui::registerWordSuggestListener(mpLayout, "word_suggest", mspSuggestionListener);

	mspMultipleButtonsListener = std::shared_ptr<MultipleButtonsListener>(new MultipleButtonsListener(this));
	eyegui::registerButtonListener(mpLayout, "space", mspMultipleButtonsListener);
	eyegui::registerButtonListener(mpLayout, "finish", mspMultipleButtonsListener);

	// Dictionary
	mDictionaryIndex = eyegui::addDictionary(mpGUI, "dictionaries/ger.txt");

	// Initialize text display
	refreshTextDisplay();
}

void TextInput::show()
{
	mText.clear();
	mCurrentWord.clear();
	refreshTextDisplay();
	eyegui::setVisibilityOfLayout(mpLayout, true, true, true);
}

void TextInput::hide()
{
	eyegui::setVisibilityOfLayout(mpLayout, false, true, true);
}

void TextInput::appendTextToCurrentWord(const std::string& text)
{
	mCurrentWord.append(text);
	eyegui::suggestWords(mpLayout, "word_suggest", mCurrentWord, mDictionaryIndex);
	refreshTextDisplay();
}

void TextInput::setCurrentWord(const std::string& text)
{
	mCurrentWord = text;
	refreshTextDisplay();
	eyegui::highlightInteractiveElement(mpLayout, "space", true);
}

void TextInput::nextWord()
{
	if (mText.empty())
	{
		mText.append(mCurrentWord);
	}
	else
	{
		mText.append(" " + mCurrentWord);
	}
	mCurrentWord.clear();
	eyegui::clearSuggestions(mpLayout, "word_suggest");
	refreshTextDisplay();
}

void TextInput::callback()
{
	nextWord(); // append current word to collected text
	mTextInputCallback(mText);
}

void TextInput::refreshTextDisplay()
{
	std::string completeText;
	if (mText.empty())
	{
		completeText = mCurrentWord + "|";
	}
	else
	{
		completeText = mText + " " + mCurrentWord + "|";
	}
	eyegui::setContentOfTextBlock(mpLayout, "textblock", completeText);
}