//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Mueller (muellerd@uni-koblenz.de)
//============================================================================

#include "DOMNode.h"

#include <iostream>

#include "src/Utils//Logger.h"

/* DOMNode methods*/
glm::vec2 DOMNode::GetCenter() const
{
	return _rect.center();
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
    "\tcoordinates: (", _rect.top, ", ", _rect.left, ", ", _rect.bottom,", ", _rect.right, ")\n",
    "\tvalue: ", _value);
}


/* DOMTextLink methods */
DOMTextLink::DOMTextLink(	DOMNodeType type,
                            int64 frameID,
                            int nodeID,
                            Rect rect,
                            std::string text,
                            std::string url) : DOMNode(type, frameID, nodeID, rect)
{
    _text = text;
    _url = url;
}
