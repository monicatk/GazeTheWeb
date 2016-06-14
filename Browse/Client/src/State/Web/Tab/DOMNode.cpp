//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel M�ller (muellerd@uni-koblenz.de)
//============================================================================

#include "DOMNode.h"

#include <iostream>

#include "src/Utils//Logger.h"

/* DOMNode methods*/
glm::vec2 DOMNode::GetCenter() const
{
    return glm::vec2((_coordinates.y + _coordinates.w) / 2.f, (_coordinates.x + _coordinates.z) / 2.f);
}

/* DOMTextInput methods*/
DOMTextInput::DOMTextInput(	DOMNodeType type,
                            int64 frameID,
                            int nodeID,
                            glm::vec4 coordinates,
                            std::string value = "") : DOMNode(type, frameID, nodeID, coordinates)
{
    _value = value;

    LogDebug("DOMTextInput constructed", "\n" ,
    "\tFrameID: ", _frameID, "\n",
    "\tnodeID: ", _nodeID, "\n",
    "\tcoordinates: (", _coordinates.x, ", ", _coordinates.y, ", ", _coordinates.z, ", ", _coordinates.w, ")\n",
    "\tvalue: ", _value);
}


/* DOMTextLink methods */
DOMTextLink::DOMTextLink(	DOMNodeType type,
                            int64 frameID,
                            int nodeID,
                            glm::vec4 coordinates,
                            std::string value,
                            std::string url) : DOMNode(type, frameID, nodeID, coordinates)
{
    _value = value;
    _url = url;
}
