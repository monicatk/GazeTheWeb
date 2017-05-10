//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Mueller (muellerd@uni-koblenz.de)
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Class for DOM nodes which are used by Tab.

#ifndef DOMNODE_H_
#define DOMNODE_H_

#include "src/CEF/Data/Rect.h"
#include "src/Utils/glmWrapper.h"
#include "src/CEF/Data/DOMAttribute.h"
#include <vector>
#include <string>
#include <memory>
#include <include/cef_process_message.h>
#include <functional> // for debugging purposes at the moment

namespace DOM
{
	void GetJSRepresentation(
		std::string nodeType,
		std::vector<const std::vector<DOMAttribute>* >& description,
		std::string& obj_getter_name
		);
}

/*
   ___  ____  __  ____  __        __      
  / _ \/ __ \/  |/  / |/ /__  ___/ /__ ___
 / // / /_/ / /|_/ /    / _ \/ _  / -_|_-<
/____/\____/_/  /_/_/|_/\___/\_,_/\__/___/
*/
class DOMNode
{
public:
	// Empty construction
	DOMNode(int id) : _id(id) {};

	// Define initialization through ICP message in each DOMNode subclass
	virtual int Initialize(CefRefPtr<CefProcessMessage> msg);
	// CefProcessMessage to C++ object
	virtual bool Update(DOMAttribute attr, CefRefPtr<CefListValue> data, 
		std::function<void (DOMNode*, DOMAttribute, CefRefPtr<CefListValue>)> f = nullptr
	);

	static void GetDescription(std::vector<const std::vector<DOMAttribute>* >* descriptions) {
		descriptions->push_back(&_description);
	}
	
	int GetId() const { return _id; }
	std::vector<Rect> GetRects() const { return _rects; }
	virtual int GetFixedId() const { return _fixedId; }
	virtual int GetOverflowId() const { return _overflowId; }

private:
	void SetId(int id) { _id = id; }
	void SetRects(std::vector<Rect> rects) { _rects = rects; }
	void SetFixedId(int fixedId) { _fixedId = fixedId; }
	void SetOverflowId(int overflowId) { _overflowId = overflowId; }

	bool IPCSetRects(CefRefPtr<CefListValue> data);
	bool IPCSetFixedId(CefRefPtr<CefListValue> data);
	bool IPCSetOverflowId(CefRefPtr<CefListValue> data);

	static const std::vector<DOMAttribute> _description;
	int _id;
	std::vector<Rect> _rects = {};
	int _fixedId = -1;				// first FixedElement's ID, which is hierarchically above this node, if any
	int _overflowId = -1;				// first DOMOverflowElement's ID, which is hierarchically above this node, if any

};


/*
   ___  ____  __  _________        __  ____               __    
  / _ \/ __ \/  |/  /_  __/____ __/ /_/  _/__  ___  __ __/ /____
 / // / /_/ / /|_/ / / / / -_) \ / __// // _ \/ _ \/ // / __(_-<
/____/\____/_/  /_/ /_/  \__/_\_\\__/___/_//_/ .__/\_,_/\__/___/
                                            /_/ 
*/

class DOMTextInput : public DOMNode
{
public:
	// Empty construction
	DOMTextInput(int id) : DOMNode(id) {};

	// Define initialization through ICP message in each DOMNode subclass
	virtual int Initialize(CefRefPtr<CefProcessMessage> msg);
	// CefProcessMessage to C++ object
	virtual bool Update(DOMAttribute attr, CefRefPtr<CefListValue> data);

	static void GetDescription(std::vector<const std::vector<DOMAttribute>* >* descriptions) {
		super::GetDescription(descriptions);
		descriptions->push_back(&_description);
	}

	static const std::string GetJSObjectGetter() {
		return "GetDOMTextInput";
	}

	std::string GetText() const { return _text; }
	bool IsPasswordField() const { return _isPassword; }

private:
	typedef DOMNode super;

	void SetText(std::string text) { _text = text; }
	void SetPassword(bool isPwd) { _isPassword = isPwd; }

	bool IPCSetText(CefRefPtr<CefListValue> data);
	bool IPCSetPassword(CefRefPtr<CefListValue> data);

	static const std::vector<DOMAttribute> _description;
	std::string _text = NULL;
	bool _isPassword = false;
};


/*
   ___  ____  __  _____   _      __      
  / _ \/ __ \/  |/  / /  (_)__  / /__ ___
 / // / /_/ / /|_/ / /__/ / _ \/  '_/(_-<
/____/\____/_/  /_/____/_/_//_/_/\_\/___/
*/

class DOMLink : public DOMNode
{
public:
	// Empty construction
	DOMLink(int id) : DOMNode(id) {};

	// Define initialization through ICP message in each DOMNode subclass
	virtual int Initialize(CefRefPtr<CefProcessMessage> msg);
	// CefProcessMessage to C++ object
	virtual bool Update(DOMAttribute attr, CefRefPtr<CefListValue> data);


	static void GetDescription(std::vector<const std::vector<DOMAttribute>* >* descriptions) {
		super::GetDescription(descriptions);
		descriptions->push_back(&_description);
	}

	static const std::string GetJSObjectGetter() {
		return "GetDOMLink";
	}

	std::string GetText() const { return _text; }
	std::string GetUrl() const { return _url; }

private:
	typedef DOMNode super;

	void SetText(std::string text) { _text = text; }
	void SetUrl(std::string url) { _url = url; }

	bool IPCSetText(CefRefPtr<CefListValue> data);
	bool IPCSetUrl(CefRefPtr<CefListValue> data);

	static const std::vector<DOMAttribute> _description;
	std::string _text = "";
	std::string _url = "";

};



/*
   __________   _______________  ______________   ___  ____
  / __/ __/ /  / __/ ___/_  __/ / __/  _/ __/ /  / _ \/ __/
 _\ \/ _// /__/ _// /__  / /   / _/_/ // _// /__/ // /\ \  
/___/___/____/___/\___/ /_/   /_/ /___/___/____/____/___/
*/

class DOMSelectField : public DOMNode
{
public:
	// Empty construction
	DOMSelectField(int id) : DOMNode(id) {};

	// Define initialization through ICP message in each DOMNode subclass
	virtual int Initialize(CefRefPtr<CefProcessMessage> msg);
	// CefProcessMessage to C++ object
	virtual bool Update(DOMAttribute attr, CefRefPtr<CefListValue> data);


	static void GetDescription(std::vector<const std::vector<DOMAttribute>* >* descriptions) {
		super::GetDescription(descriptions);
		descriptions->push_back(&_description);
	}

	static const std::string GetJSObjectGetter() {
		return "GetDOMSelectField";
	}

	std::vector<std::string> GetOptions() const { return _options; }

private:
	typedef DOMNode super;

	void SetOptions(std::vector<std::string> options) { _options = options; }

	bool IPCSetOptions(CefRefPtr<CefListValue> data);

	static const std::vector<DOMAttribute> _description;
	std::vector<std::string> _options = {};		// Entries might be NULL, if weirdly indexed on JS side (but chances are really low)
};


/*
  ____               _____           ______                   __    
 / __ \_  _____ ____/ _/ /__ _    __/ __/ /__ __ _  ___ ___  / /____
/ /_/ / |/ / -_) __/ _/ / _ \ |/|/ / _// / -_)  ' \/ -_) _ \/ __(_-<
\____/|___/\__/_/ /_//_/\___/__,__/___/_/\__/_/_/_/\__/_//_/\__/___/
*/

class DOMOverflowElement : public DOMNode
{
public:
	// Empty construction
	DOMOverflowElement(int id) : DOMNode(id) {};

	// Define initialization through ICP message in each DOMNode subclass
	virtual int Initialize(CefRefPtr<CefProcessMessage> msg);
	// CefProcessMessage to C++ object
	virtual bool Update(DOMAttribute attr, CefRefPtr<CefListValue> data);


	static void GetDescription(std::vector<const std::vector<DOMAttribute>* >* descriptions) {
		super::GetDescription(descriptions);
		descriptions->push_back(&_description);
	}

	static const std::string GetJSObjectGetter() {
		return "GetDOMOverflowElement";
	}

	std::pair<int, int> GetMaxScrolling() const { return std::make_pair(_scrollLeftMax, _scrollTopMax); }
	std::pair<int, int> GetCurrentScrolling() const { return std::make_pair(_scrollLeft, _scrollTop); }

private:
	typedef DOMNode super;

	void SetMaxScrolling(int top, int left) { _scrollTopMax = top; _scrollLeftMax = left; }
	void SetCurrentScrolling(int top, int left) { _scrollTop = top; _scrollLeft = left; }
	
	bool IPCSetMaxScrolling(CefRefPtr<CefListValue> data);
	bool IPCSetCurrentScrolling(CefRefPtr<CefListValue> data);


	static const std::vector<DOMAttribute> _description;
	int _scrollLeftMax = 0;		// maximum values for scrolling directions
	int _scrollTopMax = 0; 
	int _scrollLeft = 0;		// current position in interval [0, max value]
	int _scrollTop = 0; 
};

#endif  // DOMNODE_H_



