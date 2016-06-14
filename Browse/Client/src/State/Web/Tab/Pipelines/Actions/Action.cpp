//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "Action.h"

Action::Action(TabInteractionInterface* pTab)
{
    // Save pointer to Tab interface
    _pTab = pTab;
}

Action::~Action()
{
    // Implemented completely virtual destructor
}

void Action::SetInputValue(std::string type, int value)
{
    _inputData.SetValue(type, value);
}

void Action::SetInputValue(std::string type, int64 value)
{
	_inputData.SetValue(type, value);
}

void Action::SetInputValue(std::string type, float value)
{
    _inputData.SetValue(type, value);
}

void Action::SetInputValue(std::string type, glm::vec2 value)
{
    _inputData.SetValue(type, value);
}

void Action::SetInputValue(std::string type, std::string value)
{
    _inputData.SetValue(type, value);
}

void Action::SetInputValue(std::string type, std::u16string value)
{
	_inputData.SetValue(type, value);
}

bool Action::GetOutputValue(std::string type, int& rValue) const
{
    return _outputData.GetValue(type, rValue);
}

bool Action::GetOutputValue(std::string type, int64& rValue) const
{
	return _outputData.GetValue(type, rValue);
}

bool Action::GetOutputValue(std::string type, float& rValue) const
{
    return _outputData.GetValue(type, rValue);
}

bool Action::GetOutputValue(std::string type, glm::vec2& rValue) const
{
    return _outputData.GetValue(type, rValue);
}

bool Action::GetOutputValue(std::string type, std::string& rValue) const
{
    return _outputData.GetValue(type, rValue);
}

bool Action::GetOutputValue(std::string type, std::u16string& rValue) const
{
	return _outputData.GetValue(type, rValue);
}

void Action::AddIntInputSlot(std::string type)
{
    _inputData.AddIntSlot(type);
}

void Action::AddInt64InputSlot(std::string type)
{
	_inputData.AddInt64Slot(type);
}

void Action::AddFloatInputSlot(std::string type)
{
    _inputData.AddFloatSlot(type);
}

void Action::AddVec2InputSlot(std::string type)
{
    _inputData.AddVec2Slot(type);
}

void Action::AddStringInputSlot(std::string type)
{
    _inputData.AddStringSlot(type);
}

void Action::AddString16InputSlot(std::string type)
{
	_inputData.AddString16Slot(type);
}

void Action::AddIntOutputSlot(std::string type)
{
    _outputData.AddIntSlot(type);
}

void Action::AddInt64OutputSlot(std::string type)
{
	_outputData.AddInt64Slot(type);
}

void Action::AddFloatOutputSlot(std::string type)
{
    _outputData.AddFloatSlot(type);
}

void Action::AddVec2OutputSlot(std::string type)
{
    _outputData.AddVec2Slot(type);
}

void Action::AddStringOutputSlot(std::string type)
{
    _outputData.AddStringSlot(type);
}

void Action::AddString16OutputSlot(std::string type)
{
	_outputData.AddString16Slot(type);
}

bool Action::GetInputValue(std::string type, int& rValue) const
{
    return _inputData.GetValue(type, rValue);
}

bool Action::GetInputValue(std::string type, int64& rValue) const
{
	return _inputData.GetValue(type, rValue);
}

bool Action::GetInputValue(std::string type, float& rValue) const
{
    return _inputData.GetValue(type, rValue);
}

bool Action::GetInputValue(std::string type, glm::vec2& rValue) const
{
    return _inputData.GetValue(type, rValue);
}

bool Action::GetInputValue(std::string type, std::string& rValue) const
{
    return _inputData.GetValue(type, rValue);
}

bool Action::GetInputValue(std::string type, std::u16string& rValue) const
{
	return _inputData.GetValue(type, rValue);
}

void Action::SetOutputValue(std::string type, int value)
{
    _outputData.SetValue(type, value);
}

void Action::SetOutputValue(std::string type, int64 value)
{
	_outputData.SetValue(type, value);
}

void Action::SetOutputValue(std::string type, float value)
{
    _outputData.SetValue(type, value);
}

void Action::SetOutputValue(std::string type, glm::vec2 value)
{
    _outputData.SetValue(type, value);
}

void Action::SetOutputValue(std::string type, std::string value)
{
    _outputData.SetValue(type, value);
}

void Action::SetOutputValue(std::string type, std::u16string value)
{
	_outputData.SetValue(type, value);
}
