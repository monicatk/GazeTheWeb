//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "ActionDataMap.h"
#include "src/Utils/Logger.h"

void ActionDataMap::AddIntSlot(std::string type)
{
    _intMap[type] = ActionData<int>();
}

void ActionDataMap::AddInt64Slot(std::string type)
{
    _int64Map[type] = ActionData<int64>();
}

void ActionDataMap::AddFloatSlot(std::string type)
{
    _floatMap[type] = ActionData<float>();
}

void ActionDataMap::AddVec2Slot(std::string type)
{
    _vec2Map[type] = ActionData<glm::vec2>();
}

void ActionDataMap::AddStringSlot(std::string type)
{
    _stringMap[type] = ActionData<std::string>();
}

void ActionDataMap::AddString16Slot(std::string type)
{
    _string16Map[type] = ActionData<std::u16string>();
}

void ActionDataMap::SetValue(std::string type, int value)
{
    auto iter = _intMap.find(type);
    if(iter != _intMap.end())
    {
        iter->second.SetValue(value);
    }
    else
    {
        LogBug("No slot in ActionDataMap for ", type);
    }
}

void ActionDataMap::SetValue(std::string type, int64 value)
{
    auto iter = _int64Map.find(type);
    if (iter != _int64Map.end())
    {
        iter->second.SetValue(value);
    }
    else
    {
        LogBug("No slot in ActionDataMap for ", type);
    }
}

void ActionDataMap::SetValue(std::string type, float value)
{
    auto iter = _floatMap.find(type);
    if(iter != _floatMap.end())
    {
        iter->second.SetValue(value);
    }
    else
    {
        LogBug("No slot in ActionDataMap for ", type);
    }
}

void ActionDataMap::SetValue(std::string type, glm::vec2 value)
{
    auto iter = _vec2Map.find(type);
    if(iter != _vec2Map.end())
    {
        iter->second.SetValue(value);
    }
    else
    {
        LogBug("No slot in ActionDataMap for ", type);
    }
}

void ActionDataMap::SetValue(std::string type, std::string value)
{
    auto iter = _stringMap.find(type);
    if(iter != _stringMap.end())
    {
        iter->second.SetValue(value);
    }
    else
    {
        LogBug("No slot in ActionDataMap for ", type);
    }
}

void ActionDataMap::SetValue(std::string type, std::u16string value)
{
    auto iter = _string16Map.find(type);
    if (iter != _string16Map.end())
    {
        iter->second.SetValue(value);
    }
    else
    {
        LogBug("No slot in ActionDataMap for ", type);
    }
}

bool ActionDataMap::GetValue(std::string type, int& rValue) const
{
    auto iter = _intMap.find(type);
    if(iter != _intMap.end())
    {
        rValue = iter->second.GetValue();
        return iter->second.IsFilled();
    }
    else
    {
        LogBug("No slot in ActionDataMap for ", type);
    }
    return false;
}

bool ActionDataMap::GetValue(std::string type, int64& rValue) const
{
    auto iter = _int64Map.find(type);
    if (iter != _int64Map.end())
    {
        rValue = iter->second.GetValue();
        return iter->second.IsFilled();
    }
    else
    {
        LogBug("No slot in ActionDataMap for ", type);
    }
    return false;
}

bool ActionDataMap::GetValue(std::string type, float& rValue) const
{
    auto iter = _floatMap.find(type);
    if(iter != _floatMap.end())
    {
        rValue = iter->second.GetValue();
        return iter->second.IsFilled();
    }
    else
    {
        LogBug("No slot in ActionDataMap for ", type);
    }
    return false;
}

bool ActionDataMap::GetValue(std::string type, glm::vec2& rValue) const
{
    auto iter = _vec2Map.find(type);
    if(iter != _vec2Map.end())
    {
        rValue = iter->second.GetValue();
        return iter->second.IsFilled();
    }
    else
    {
        LogBug("No slot in ActionDataMap for ", type);
    }
    return false;
}

bool ActionDataMap::GetValue(std::string type, std::string& rValue) const
{
    auto iter = _stringMap.find(type);
    if(iter != _stringMap.end())
    {
        rValue = iter->second.GetValue();
        return iter->second.IsFilled();
    }
    else
    {
        LogBug("No slot in ActionDataMap for ", type);
    }
    return false;
}

bool ActionDataMap::GetValue(std::string type, std::u16string& rValue) const
{
    auto iter = _string16Map.find(type);
    if (iter != _string16Map.end())
    {
        rValue = iter->second.GetValue();
        return iter->second.IsFilled();
    }
    else
    {
        LogBug("No slot in ActionDataMap for ", type);
    }
    return false;
}
