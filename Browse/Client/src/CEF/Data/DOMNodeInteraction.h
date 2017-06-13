//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Mueller (muellerd@uni-koblenz.de)
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#ifndef DOMNODEINTERACTION_H_
#define DOMNODEINTERACTION_H_

#include <include/cef_browser.h>
#include <functional>

class Tab;	// Forward declaration

class DOMBaseInterface
{
public:

	// Required in order to being able to access corresponding JS DOM object
	virtual int GetId() = 0;
	virtual int GetType() = 0;
};

/*
 * Guarantee, that DOMNode provides any informationen needed in order to contact
 * its corresponding Javascript DOM node object
*/
class DOMJavascriptCommunication  : public virtual DOMBaseInterface
{
public:
	DOMJavascriptCommunication(Tab* pTab) :
		_pTab(pTab) {};

	bool SendProcessMessageToRenderer(CefRefPtr<CefProcessMessage> msg);

	// Helper
	CefRefPtr<CefProcessMessage> SetupExecuteFunctionMessage(std::string func_name,
		CefRefPtr<CefListValue> param);

	Tab* _pTab;

};

class DOMTextInputInteraction : public virtual DOMJavascriptCommunication
{
public:
	DOMTextInputInteraction(Tab* pTab) {};

	// Send IPC message to JS in order to execute text input function
	void InputText(std::string text, bool submit);
};

class DOMOverflowElementInteraction : public virtual DOMJavascriptCommunication
{
public:
	DOMOverflowElementInteraction(Tab* pTab) {};

	// Send IPC message to JS in order to execute scrolling function
	void Scroll(int x, int y, std::vector<int> fixedIds = {});
};

class DOMSelectFieldInteraction : public virtual DOMJavascriptCommunication
{
public:
	DOMSelectFieldInteraction(Tab* pTab) {};

	// Send IPC message to JS in order to execute JS function
	void SetSelectionIndex(int idx);
};


#endif // DOMNODEINTERACTION_H_

/*
OLD APPROACH
// Needed additional definition of TextInput::getId linking to DOMNode::getId ...

class DOMNodeIdentification
{
public:
virtual int GetId() = 0;
virtual int GetType() = 0;
};

class DOMJavascriptCommunication
{
public:
DOMJavascriptCommunication(CefRefPtr<CefBrowser> browser) :
_browser(browser) {};

CefRefPtr<CefBrowser> GetBrowser() { return _browser; }

CefRefPtr<CefBrowser> _browser;
};

class DOMTextInputInteraction : public DOMNodeIdentification
{
public:
virtual CefRefPtr<CefBrowser> GetBrowser() = 0;

// Send IPC message to JS in order to execute text input function
void InputText(std::string text, bool submit);
};
*/