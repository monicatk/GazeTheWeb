//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "src/State/Web/Tab/Pipelines/Actions/ActionConnector.h"

ActionConnector::ActionConnector(std::weak_ptr<const Action> wpPrevious, std::weak_ptr<Action> wpNext)
{
    _wpPrevious = wpPrevious;
    _wpNext = wpNext;
}

void ActionConnector::Execute()
{
    // Connect per datatype
	Execute<int>(_intConnections);
	Execute<float>(_floatConnections);
	Execute<glm::vec2>(_vec2Connections);
	Execute<std::string>(_stringConnections);
	Execute<std::u16string>(_string16Connections);
}

void ActionConnector::ConnectInt(std::string previousType, std::string nextType)
{
   _intConnections[previousType] = nextType;
}

void ActionConnector::ConnectFloat(std::string previousType, std::string nextType)
{
    _floatConnections[previousType] = nextType;
}

void ActionConnector::ConnectVec2(std::string previousType, std::string nextType)
{
    _vec2Connections[previousType] = nextType;
}

void ActionConnector::ConnectString(std::string previousType, std::string nextType)
{
    _stringConnections[previousType] = nextType;
}

void ActionConnector::ConnectString16(std::string previousType, std::string nextType)
{
	_string16Connections[previousType] = nextType;
}