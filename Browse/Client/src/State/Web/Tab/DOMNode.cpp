//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Mueller (muellerd@uni-koblenz.de)
//============================================================================

#include "DOMNode.h"

#include <iostream>

#include "src/Utils//Logger.h"

/* DOMNode methods*/

// Get list of center points as vec2 of all Rects in list
std::vector<glm::vec2> DOMNode::GetCenters() const
{
	std::vector<glm::vec2> centers;
	for (int i = 0; i < _rects.size(); i++)
	{
		centers.push_back(_rects[i].center());
	}
	return centers;
}

glm::vec2 DOMNode::GetCenter() const
{
	glm::vec2 center(0,0);
	if (_rects.size() > 0)
	{
		for (int i = 0; i < _rects.size(); i++)
		{
			center += _rects[i].center();
		}
		center /= _rects.size();
	}
	return center;
}

void DOMNode::UpdateAttribute(int attr, void * data, bool initial)
{
	switch (attr)
	{
		// Update Rect
		case(0) : {
			// TODO: There might be a problem, when initial data from Renderer arrives after an update
			// --> bool attribute for node if Renderer data already received instead?
			if (!initial || _rects[0].isZero())
			{
				_rects[0] = *(Rect*)data;

				//if (_rect.width() > 0 && _rect.height() > 0)
				//	LogDebug("type=", _type, "; id=", _nodeID, "; _fixed=", _fixed, "; rect= ", _rect.toString());
			}

			break;
		}
		case(1) : {
			_fixed = *(bool*)data;
			
			//if (_rect.width() > 0 && _rect.height() > 0)
			//	LogDebug("type=", _type, "; id=", _nodeID, "; _fixed=", _fixed, "; rect= ", _rect.toString());
			
			break;
		}
		// Add new Rect (atm only for DOMLinks)
		case(2): {
			AddRect(*(Rect*)data);
			break;
		}
		case(3) : {
			_rects = *(std::vector<Rect>*) data;

			//for (int i = 0; i < _rects.size(); i++)
			//{
			//	LogDebug("DOMNode: ",i, ": ", _rects[i].toString());
			//}

			break;
		}
		default: {
			LogDebug("DOMNode: Update for attribute=", attr, " is not yet defined!");
		}

	}
	// TODO: More cases for other attributes
}



/* DOMTextInput methods*/
DOMTextInput::DOMTextInput(	DOMNodeType type,
                            int64 frameID,
                            int nodeID,
                            Rect rect,
                            std::string value = "") : DOMNode(type, frameID, nodeID, rect)
{
    _value = value;

    LogDebug("DOMTextInput constructed", "\n" ,
    "\tFrameID: ", _frameID, "\n",
    "\tnodeID: ", _nodeID, "\n",
    "\tcoordinates: ", _rects[0].toString(), "\n",
    "\tvalue: ", _value);
}


/* DOMTextLink methods */
DOMLink::DOMLink(	DOMNodeType type,
                            int64 frameID,
                            int nodeID,
                            Rect rect,
                            std::string text,
                            std::string url) : DOMNode(type, frameID, nodeID, rect)
{
    _text = text;
    _url = url;
}

DOMLink::DOMLink(
	DOMNodeType type, 
	int64 frameID, 
	int nodeID, 
	std::vector<Rect> rects, 
	std::string text, 
	std::string url) : DOMNode(type, frameID, nodeID, rects)
{
	_text = text;
	_url = url;
}
