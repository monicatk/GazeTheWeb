//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Mueller (muellerd@uni-koblenz.de)
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Abstract interfaces for interaction with DOM nodes.

#ifndef DOMNODEINTERACTION_H_
#define DOMNODEINTERACTION_H_

#include "include/cef_browser.h"
#include "src/CEF/Data/DOMAttribute.h"
#include "src/Utils/Logger.h"
#include <functional>

class Tab;	// Forward declaration

typedef std::function<bool(CefRefPtr<CefProcessMessage>)> SendRenderMessage;

// TODO: Strange place (this header file) for BaseInterface?
class DOMBaseInterface
{
public:

	// Required in order to being able to access corresponding JS DOM object
	virtual int GetId() = 0;
	virtual int GetType() = 0;

	// Enable accessing attribute data while running the program
	virtual bool PrintAttribute(DOMAttribute attr) = 0;
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
        _sendRenderMessage(sendRenderMessage) {}

	// Sending message to renderer
	bool SendProcessMessageToRenderer(CefRefPtr<CefProcessMessage> msg);


	/* ### Helpers ### */

	// Maps given data type to CefListValue function in order to set idx to this value
	template<typename T>
	void AppendToList(CefRefPtr<CefListValue> list, T val) { LogInfo("DOMJavascriptCommunication: Unhandled data type!"); };

	void AppendToList(CefRefPtr<CefListValue> list, bool val) 
	{ 
		list->SetBool(list->GetSize(), val); 
	};

	void AppendToList(CefRefPtr<CefListValue> list, double val) 
	{ 
		list->SetDouble(list->GetSize(), val);
	};

	void AppendToList(CefRefPtr<CefListValue> list, int val) 
	{ 
		list->SetInt(list->GetSize(), val);
	};

	void AppendToList(CefRefPtr<CefListValue> list, std::string val) 
	{ 
		list->SetString(list->GetSize(), val);
	};

	template<typename T>
	void AppendToList(CefRefPtr<CefListValue> list, std::vector<T> val)
	{
		CefRefPtr<CefListValue> sublist = CefListValue::Create();
		for (const auto subval : val)
			AppendToList(sublist, subval);
		list->SetList(list->GetSize(), sublist);
	};

	// Add an arbitrarily typed value to a CefListValue
	template<typename T>
	void AddToList(CefRefPtr<CefListValue> list, T val)
	{
		AppendToList(list, val);
	};

	// Add several arbitrarily typed values to a CefListValue
	template<typename T, typename... Args>
	void AddToList(CefRefPtr<CefListValue> list, T val, Args... args)
	{
		AppendToList(list, val);
		AddToList(list, args...);
	}

	// Given a JS function name and a list of parameters, a process message is send to JS, which executes given
	// node's function with the provided parameters
	template<typename T, typename... Args>
	void SendExecuteFunctionMessage(std::string func_name, T param, Args... args)
	{
		CefRefPtr<CefListValue> params = CefListValue::Create();
		AddToList<T>(params, param, args...);
		CefRefPtr<CefProcessMessage> msg = SetupExecuteFunctionMessage(func_name, params);
		SendProcessMessageToRenderer(msg);
	}
	// TODO: Is this function really neccessary?
	template<typename T>
	void SendExecuteFunctionMessage(std::string func_name, T param)
	{
		CefRefPtr<CefListValue> params = CefListValue::Create();
		AddToList<T>(params, param);
		CefRefPtr<CefProcessMessage> msg = SetupExecuteFunctionMessage(func_name, params);
		SendProcessMessageToRenderer(msg);
	}

	// Setup process message by adding a header (node id and type) and given params to its argument list
	CefRefPtr<CefProcessMessage> SetupExecuteFunctionMessage(
		std::string func_name,
		CefRefPtr<CefListValue> param);



	// Member
	SendRenderMessage _sendRenderMessage;

private:

    // Default constructor only for friends. Problem: Virtual classes have to call some
    // constructor, although never instantiated on their own. Finally utilized classes like
    // DOMTextInput are calling the "good" constructor with parameter. So below is never really used.
    DOMJavascriptCommunication();

    // This are the friends who have to access the default constructor
    friend class DOMTextInputInteraction;
    friend class DOMOverflowElementInteraction;
    friend class DOMSelectFieldInteraction;
	friend class DOMVideoInteraction;

};

// Interaction with text input
class DOMTextInputInteraction : public virtual DOMJavascriptCommunication
{
public:

	// Constructor
    DOMTextInputInteraction() {}

	// Send IPC message to JS in order to execute text input function
	void InputText(std::string text, bool submit){ SendExecuteFunctionMessage("inputText", submit); }
};

// Interaction with overflow element
class DOMOverflowElementInteraction : public virtual DOMJavascriptCommunication
{
public:

	// Constructor
    DOMOverflowElementInteraction() {}

	// TODO taking gaze, should take scrolling offset
	// Send IPC message to JS in order to execute scrolling function
	void Scroll(int x, int y, std::vector<int> fixedIds = {}) { SendExecuteFunctionMessage("scroll", x, y, fixedIds); }
};

// Interaction with select field
class DOMSelectFieldInteraction : public virtual DOMJavascriptCommunication
{
public:

	// Constructor
    DOMSelectFieldInteraction() {}

	// Send IPC message to JS in order to execute JS function
	void SetSelectionIndex(int idx) { SendExecuteFunctionMessage("setSelectionIndex", idx); }
};

// Interaction with videos
class DOMVideoInteraction : public virtual DOMJavascriptCommunication
{
public:

	// Constructor
	DOMVideoInteraction() {}

	// Send IPC message to JS in order to execute JS function
	void JumpToSecond(float sec = 0.f) { SendExecuteFunctionMessage("jumptToSecond", sec); }
	void SetPlaying(bool playing = true) { SendExecuteFunctionMessage("setPlaying", playing); }
	void SetMuted(bool muted = true) { SendExecuteFunctionMessage("setMuted", muted); }
	void SetVolume(float volume) { SendExecuteFunctionMessage("setVolume", volume); }
	void ShowControls(bool show = true) { SendExecuteFunctionMessage("showControls", show); }
	void SetFullscreen(bool fullscreen = true) { SendExecuteFunctionMessage("setFullscreen", fullscreen); }
};


#endif // DOMNODEINTERACTION_H_
