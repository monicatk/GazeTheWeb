//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Connects output ActionDataMap of one Action with input ActionDataMap of other.

#ifndef ACTIONCONNECTOR_H_
#define ACTIONCONNECTOR_H_

#include "src/State/Web/Tab/Pipelines/Actions/Action.h"
#include <map>

class ActionConnector
{
public:

    // Constructor
    ActionConnector(Action const * pPrevious, Action* pNext);

    // Execute connection (should be done after previous action is executed and next is about to be)
    void Execute();

    // Get pointer to previous to decide about execution
    Action const * GetPreviousAction() const { return _pPrevious; }

    // Connect
    void ConnectInt(std::string previousType, std::string nextType);
    void ConnectFloat(std::string previousType, std::string nextType);
    void ConnectVec2(std::string previousType, std::string nextType);
    void ConnectString(std::string previousType, std::string nextType);
    void ConnectString16(std::string previousType, std::string nextType);

private:

    // Pointer to actions
    Action const * _pPrevious;
    Action* _pNext;

    // Map of connections
    std::map<std::string, std::string> _intConnections;
    std::map<std::string, std::string> _floatConnections;
    std::map<std::string, std::string> _vec2Connections;
    std::map<std::string, std::string> _stringConnections;
    std::map<std::string, std::string> _string16Connections;
};

#endif // ACTIONCONNECTOR_H_
