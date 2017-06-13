//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Mueller (muellerd@uni-koblenz.de)
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Abstract interfaces for interaction with DOM nodes.

#ifndef DOMNODEINTERACTION_H_
#define DOMNODEINTERACTION_H_

#include "include/cef_browser.h"
#include <functional>

class Tab;	// Forward declaration

typedef std::function<bool(CefRefPtr<CefProcessMessage>)> SendRenderMessage;

class DOMBaseInterface
{
public:

	// Required in order to being able to access corresponding JS DOM object
	virtual int GetId() = 0;
	virtual int GetType() = 0;
};

/*
 * Guarantee, that DOMNode provides any informationen needed in order to contact
 * its corresponding JavaScript DOM node object
*/
class DOMJavascriptCommunication  : public virtual DOMBaseInterface
{
public:

	// Constructor
	DOMJavascriptCommunication(SendRenderMessage sendRenderMessage) :
		_sendRenderMessage(sendRenderMessage) {};

	// Sending message to renderer
	bool SendProcessMessageToRenderer(CefRefPtr<CefProcessMessage> msg);

	// Helper
	CefRefPtr<CefProcessMessage> SetupExecuteFunctionMessage(
		std::string func_name,
		CefRefPtr<CefListValue> param);

	// Member
	SendRenderMessage _sendRenderMessage;

};

// Interaction with text input
class DOMTextInputInteraction : public virtual DOMJavascriptCommunication
{
public:

	// Constructor
	DOMTextInputInteraction() {};

	// Send IPC message to JS in order to execute text input function
	void InputText(std::string text, bool submit);
};

// Interaction with overflow element
class DOMOverflowElementInteraction : public virtual DOMJavascriptCommunication
{
public:

	// Constructor
	DOMOverflowElementInteraction() {};

	// TODO taking gaze, should take scrolling offset
	// Send IPC message to JS in order to execute scrolling function
	void Scroll(int x, int y, std::vector<int> fixedIds = {});
};

// Interaction with select field
class DOMSelectFieldInteraction : public virtual DOMJavascriptCommunication
{
public:

	// Constructor
	DOMSelectFieldInteraction() {};

	// Send IPC message to JS in order to execute JS function
	void SetSelectionIndex(int idx);
};

#endif // DOMNODEINTERACTION_H_