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
    // Connect ints
    for(const auto& rConnection : _intConnections)
    {
        int value;
		if (auto spPrevious = _wpPrevious.lock()) { spPrevious->GetOutputValue(rConnection.first, value); }
		if (auto spNext = _wpNext.lock()) { spNext->SetInputValue(rConnection.second, value); }
    }

    // Connect floats
    for(const auto& rConnection : _floatConnections)
    {
        float value;
		if (auto spPrevious = _wpPrevious.lock()) { spPrevious->GetOutputValue(rConnection.first, value); }
		if (auto spNext = _wpNext.lock()) { spNext->SetInputValue(rConnection.second, value); }
    }

    // Connect vec2s
    for(const auto& rConnection : _vec2Connections)
    {
        glm::vec2 value;
		if (auto spPrevious = _wpPrevious.lock()) { spPrevious->GetOutputValue(rConnection.first, value); }
		if (auto spNext = _wpNext.lock()) { spNext->SetInputValue(rConnection.second, value); }
    }

    // Connect strings
    for(const auto& rConnection : _stringConnections)
    {
        std::string value;
		if (auto spPrevious = _wpPrevious.lock()) { spPrevious->GetOutputValue(rConnection.first, value); }
		if (auto spNext = _wpNext.lock()) { spNext->SetInputValue(rConnection.second, value); }
    }

	// Connect u16strings
	for (const auto& rConnection : _string16Connections)
	{
		std::u16string value;
		if (auto spPrevious = _wpPrevious.lock()) { spPrevious->GetOutputValue(rConnection.first, value); }
		if (auto spNext = _wpNext.lock()) { spNext->SetInputValue(rConnection.second, value); }
	}
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