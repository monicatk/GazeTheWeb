//============================================================================
// Distributed under the MIT License.
// Author: Raphael Menges (https://github.com/raphaelmenges)
//============================================================================

#ifndef TEXT_INPUT_H
#define TEXT_INPUT_H

#include "externals/eyeGUI/include/eyeGUI.h"

// Forward declaration
class TextInput;

// Keyboard listener
class InputListener : public eyegui::KeyboardListener
{
public:

	// Constructor
	InputListener(TextInput* pTextInput);

	// Callbacks
	void virtual keyPressed(eyegui::Layout* pLayout, std::string id, std::u16string value) {}
	void virtual keyPressed(eyegui::Layout* pLayout, std::string id, std::string value);

private:

	// Members
	TextInput* mpTextInput;
};

// Word suggestion listener
class SuggestionListener : public eyegui::WordSuggestListener
{
public:

	// Constructor
	SuggestionListener(TextInput* pTextInput);

	// Callbacks
	void virtual chosen(eyegui::Layout* pLayout, std::string id, std::u16string value) {}
	void virtual chosen(eyegui::Layout* pLayout, std::string id, std::string value);

private:

	// Members
	TextInput* mpTextInput;
};

// Button listener
class MultipleButtonsListener : public eyegui::ButtonListener
{
public:

	// Constructor
	MultipleButtonsListener(TextInput* pTextInput);

	// Callbacks
	void virtual hit(eyegui::Layout* pLayout, std::string id);
	void virtual down(eyegui::Layout* pLayout, std::string id);
	void virtual up(eyegui::Layout* pLayout, std::string id);

private:

	// Members
	TextInput* mpTextInput;
};

// Class
class TextInput
{
public:

	// Constructor
	TextInput(eyegui::GUI* pGUI, void(*textInputCallback) (std::string));

	// Show text input
	void show();

	// Hide text input
	void hide();

	// Append text to current word
	void appendTextToCurrentWord(const std::string& text);

	// Set current word
	void setCurrentWord(const std::string& text);

	// Prepare for next word
	void nextWord();

	// Call back owner
	void callback();

private:

	// Refresh text (just the displaying of it)
	void refreshTextDisplay();

	// Members
	eyegui::GUI* mpGUI;
	eyegui::Layout* mpLayout;
	std::string mCurrentWord;
	std::string mText;
	std::shared_ptr<InputListener> mspInputListener;
	std::shared_ptr<SuggestionListener> mspSuggestionListener;
	std::shared_ptr<MultipleButtonsListener> mspMultipleButtonsListener;
	int mDictionaryIndex;
	void(*mTextInputCallback) (std::string);
};

#endif // TEXT_INPUT_H