//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "src/State/Web/Tab/Pipelines/Actions/ActionConnector.h"

ActionConnector::ActionConnector(Action const * pPrevious, Action* pNext)
{
    _pPrevious = pPrevious;
    _pNext = pNext;
}

void ActionConnector::Execute()
{
    // Connect ints
    for(const auto& rConnection : _intConnections)
    {
        int value;
        _pPrevious->GetOutputValue(rConnection.first, value);
        _pNext->SetInputValue(rConnection.second, value);
    }

    // Connect floats
    for(const auto& rConnection : _floatConnections)
    {
        float value;
        _pPrevious->GetOutputValue<float>(rConnection.first, value);
        _pNext->SetInputValue(rConnection.second, value);
    }

    // Connect vec2s
    for(const auto& rConnection : _vec2Connections)
    {
        glm::vec2 value;
        _pPrevious->GetOutputValue(rConnection.first, value);
        _pNext->SetInputValue(rConnection.second, value);
    }

    // Connect strings
    for(const auto& rConnection : _stringConnections)
    {
        std::string value;
        _pPrevious->GetOutputValue(rConnection.first, value);
        _pNext->SetInputValue(rConnection.second, value);
    }

	// Connect u16strings
	for (const auto& rConnection : _string16Connections)
	{
		std::u16string value;
		_pPrevious->GetOutputValue(rConnection.first, value);
		_pNext->SetInputValue(rConnection.second, value);
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