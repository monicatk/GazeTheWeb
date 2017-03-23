//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Mueller (muellerd@uni-koblenz.de)
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Class for DOM nodes which are used by Tab.

#ifndef DOMNODE_H_
#define DOMNODE_H_

//#include "src/Typedefs.h"
#include "src/CEF/Data/Rect.h"
#include "src/Utils/glmWrapper.h"
#include "src/CEF/Data/DOMExtraction.h"
//#include "src/CEF/Data/DOMNodeType.h"
#include <vector>
#include <string>
#include <memory>
#include <include/cef_process_message.h>


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
	virtual bool Update(DOMAttribute attr, CefRefPtr<CefListValue> data);

	const std::vector<DOMAttribute> GetDescription();
	
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
	int _fixedId = NULL;				// first FixedElement's ID, which is hierarchically above this node, if any
	int _overflowId = NULL;				// first OverflowElement's ID, which is hierarchically above this node, if any

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


	std::string GetText() const { return _text; }
	std::string GetUrl() const { return _url; }

private:
	typedef DOMNode super;

	void SetText(std::string text) { _text = text; }
	void SetUrl(std::string url) { _url = url; }

	bool IPCSetText(CefRefPtr<CefListValue> data);
	bool IPCSetUrl(CefRefPtr<CefListValue> data);

	static const std::vector<DOMAttribute> _description;
	std::string _text = NULL;
	std::string _url = NULL;

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

class OverflowElement : public DOMNode
{
public:
	// Empty construction
	OverflowElement(int id) : DOMNode(id) {};

	std::pair<int, int> GetMaxScrolling() const { return std::make_pair(_scrollLeftMax, _scrollTopMax); }
	std::pair<int, int> GetCurrentScrolling() const { return std::make_pair(_scrollLeft, _scrollTop); }

	void SetMaxScrolling(int top, int left) { _scrollTopMax = top; _scrollLeftMax = left; }
	void SetCurrentScrolling(int top, int left) { _scrollTop = top; _scrollLeft = left; }

private:
	int _scrollLeftMax = 0;		// maximum values for scrolling directions
	int _scrollTopMax = 0; 
	int _scrollLeft = 0;		// current position in interval [0, max value]
	int _scrollTop = 0; 
};

#endif  // DOMNODE_H_

//
//// General DOM node
//class DOMNode
//{
//public:
//
//	DOMNodeType GetType() const { return _type; } // TODO: Get rid of type, maybe virtual GetType method for every inheriting class 
//												  // with constant return value
//												  // BUT: type needed for GetDOMNode in Tab!
//	int64 GetFrameID() const { return _frameID; }
//	int GetNodeID() const { return _nodeId; };
//	std::vector<Rect> GetRects() const { return _rects; };
//	glm::vec2 GetCenter() const; // unified center of all bounding rects
//	std::vector<glm::vec2> GetCenters() const;
//	bool GetFixed() const { return _fixed; }
//	bool GetVisibility() const { return _visible; }
//	const std::string GetText() const { return _text; }
//	bool IsPasswordField() const { return _isPasswordField; }
//	void AddRect(Rect rect) { _rects.push_back(rect); }
//	void SetRects(std::shared_ptr<std::vector<Rect>> rects);
//	void SetFixed(bool fixed) { _fixed = fixed; }
//	void SetVisibility(bool visible) { _visible = visible; }
//	void SetText(std::string text) { _text = text; }
//	void SetAsPasswordField() { _isPasswordField = true; }
//
//	// TODO(Daniel): Make DOMNode class purely abstract, Get rid of type attribute
//	// blank constructor
//	DOMNode(DOMNodeType type, int nodeId)
//	{
//		_nodeId = nodeId;
//		_type = type;
//	}
//
//	// Constructor for nodes with single bounding rect
//	DOMNode(DOMNodeType type, int64 frameID, int nodeID, Rect rect)
//	{
//		_type = type;
//		_frameID = frameID;
//		_nodeId = nodeID;
//		_rects = { rect };
//	}
//
//	// Constructor for node with multiple bounding rects
//	DOMNode(DOMNodeType type, int64 frameID, int nodeID, std::vector<Rect> rects)
//	{
//		_type = type;
//		_frameID = frameID;
//		_nodeId = nodeID;
//		_rects = rects;
//	}
//
//protected:
//
//	// Members
//	DOMNodeType _type;
//	int64 _frameID;
//	int _nodeId; // Node's position in JavaScript's list of nodes of the same type
//	std::vector<Rect> _rects = { };
//	bool _fixed = false;
//	bool _visible = true;
//	std::string _text = "";
//	bool _isPasswordField = false;
//};
//
//// DOM node of text input field
//class DOMTextInput : public DOMNode
//{
//public:
//
//	// Constructor
//	DOMTextInput(
//		DOMNodeType type,
//		int64 frameID,
//		int nodeID,
//		Rect rect,
//		std::string value = "");
//
//	std::string GetValue() const { return _value; }
//
//private:
//
//	// Members
//	std::string _value = "";
//};
//
//// DOM node of link
//class DOMLink : public DOMNode
//{
//public:
//
//	// Constructor for single bounding rect
//	DOMLink(
//		DOMNodeType type,
//		int64 frameID,
//		int nodeID,
//		Rect rect,
//		std::string url);
//
//	// Constructor for multiple bounding rects
//	DOMLink(
//		DOMNodeType type,
//		int64 frameID,
//		int nodeID,
//		std::vector<Rect> rects,
//		std::string url);
//
//	// Getter of URL in link
//	std::string GetURL() const { return _url; }
//
//private:
//
//	// Members
//	std::string _url;
//};
//
//class DOMSelectField : public DOMNode
//{
//public:
//	DOMSelectField(		// TODO: This constructor won't be used?
//		int nodeId,
//		std::vector<Rect> rects,
//		std::vector<std::string> options) : DOMNode(DOMNodeType::SelectField, -1, nodeId, rects)
//	{
//		_options = options;
//	}
//
//
//};

// Overflow element




