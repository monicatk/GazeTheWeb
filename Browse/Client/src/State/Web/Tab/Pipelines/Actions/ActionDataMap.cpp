//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "ActionDataMap.h"


void ActionDataMap::AddIntSlot(std::string type, int value)
{
    _intMap[type] = std::make_unique<ActionData<int> >(value);
}

void ActionDataMap::AddInt64Slot(std::string type, int64 value)
{
    _int64Map[type] = std::make_unique<ActionData<int64> >(value);
}

void ActionDataMap::AddFloatSlot(std::string type, float value)
{
    _floatMap[type] = std::make_unique<ActionData<float> >(value);
}

void ActionDataMap::AddVec2Slot(std::string type, glm::vec2 value)
{
    _vec2Map[type] = std::make_unique<ActionData<glm::vec2> >(value);
}

void ActionDataMap::AddStringSlot(std::string type, std::string value)
{
    _stringMap[type] = std::make_unique<ActionData<std::string> >(value);
}

void ActionDataMap::AddString16Slot(std::string type, std::u16string value)
{
    _string16Map[type] = std::make_unique<ActionData<std::u16string> >(value);
}