//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Mueller (muellerd@uni-koblenz.de)
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "DOMNode.h"
#include "src/Utils/Logger.h"
#include "src/Utils/Helper.h"
#include <algorithm>

/*
   ___  ____  __  ____  __        __      
  / _ \/ __ \/  |/  / |/ /__  ___/ /__ ___
 / // / /_/ / /|_/ /    / _ \/ _  / -_|_-<
/____/\____/_/  /_/_/|_/\___/\_,_/\__/___/
*/
const std::vector<DOMAttribute> DOMNode::_description = {
	Rects, FixedId, OverflowId
};

int DOMNode::Initialize(CefRefPtr<CefProcessMessage> msg)
{
	const auto args = msg->GetArgumentList();
	if (args->GetSize() < _description.size() + 1)
	{
		LogError("DOMNode: On initialization: Object description and message size do not match!");
	}
	else
	{
		for (int i = 0; i < _description.size(); i++)
		{
			if (!Update(_description[i], args->GetValue(i + 1)))
			{
				LogError("DOMNode: Failed to assign value of type ", args->GetValue(i + 1)->GetType(),
					" to attribute ", _description[i], "!");
			}
		}
	}
	return _description.size() + 1;
}

bool DOMNode::Update(DOMAttribute attr, CefRefPtr<CefListValue> data)
{
	switch (attr) {
		case DOMAttribute::Rects:			return IPCSetRects(data);
		case DOMAttribute::FixedId:			return IPCSetFixedId(data);
		case DOMAttribute::OverflowId:		return IPCSetOverflowId(data);
	}
	LogError("DOMNode: Could not find attribute ", attr, " in order to assign data");
	return false;
}

bool DOMNode::IPCSetRects(CefRefPtr<CefListValue> data)
{
	// TODO: Unterscheidung CefListValue und CefValue...
	if (data->GetSize() != 1 && data->GetValue(0)->GetType() != CefValueType::VTYPE_LIST)
		return false;

	const auto rectList = data->GetValue(0)->GetList();
	std::vector<Rect> rects;
	for (int i = 0; i < rectList->GetSize(); i++)
	{
		const auto rectData = rectList->GetValue(i)->GetList();
		std::vector<float> rect;
		for (int j = 0; j < rectData->GetSize(); j++)
		{
			rect.push_back(rectData->GetValue(j)->GetDouble());
		}
		rects.push_back(Rect(rect));
	}

	SetRects(rects);
	return true;
}

bool DOMNode::IPCSetFixedId(CefRefPtr<CefListValue> data)
{
	if (data->GetSize() < 1 || data->GetType(0) != CefValueType::VTYPE_INT)
		return false;

	SetFixedId(data->GetInt(0));
	return true;
}

bool DOMNode::IPCSetOverflowId(CefRefPtr<CefListValue> data)
{
	if (data->GetSize() < 1 || data->GetType(0) != CefValueType::VTYPE_INT)
		return false;

	SetOverflowId(data->GetInt(0));
	return true;
}

/*
   ___  ____  __  _________        __  ____               __    
  / _ \/ __ \/  |/  /_  __/____ __/ /_/  _/__  ___  __ __/ /____
 / // / /_/ / /|_/ / / / / -_) \ / __// // _ \/ _ \/ // / __(_-<
/____/\____/_/  /_/ /_/  \__/_\_\\__/___/_//_/ .__/\_,_/\__/___/
                                            /_/ 
*/
const std::vector<DOMAttribute> DOMTextInput::_description = {
	Text, IsPassword
};

int DOMTextInput::Initialize(CefRefPtr<CefProcessMessage> msg)
{
	// First list element to start interpretation as this class's attributes
	int pivot = super::Initialize(msg);

	const auto args = msg->GetArgumentList();
	if (args->GetSize() < _description.size() + pivot)
	{
		LogError("DOMTextInput: On initialization: Object description and message size do not match!");
	}
	else
	{
		for (int i = 0; i < _description.size(); i++)
		{
			if (!Update(_description[i], args->GetValue(i + 1)))
			{
				LogError("DOMTextInput: Failed to assign value of type ", args->GetValue(i + 1)->GetType(),
					" to attribute ", _description[i], "!");
			}
		}
	}
	return _description.size() + pivot;
}

bool DOMTextInput::Update(DOMAttribute attr, CefRefPtr<CefListValue> data)
{
	switch (attr) {
		case DOMAttribute::Text:			return IPCSetText(data);
		case DOMAttribute::IsPassword:		return IPCSetPassword(data);
	}
	return super::Update(attr, data);
}

bool DOMTextInput::IPCSetText(CefRefPtr<CefListValue> data)
{
	if (data->GetSize() < 1 || data->GetValue(0) ->GetType() != CefValueType::VTYPE_STRING)
		return false;

	SetText(data->GetString(0));
	return true;
}

bool DOMTextInput::IPCSetPassword(CefRefPtr<CefListValue> data)
{
	if (data->GetSize() < 1 || data->GetValue(0)->GetType() != CefValueType::VTYPE_BOOL)
		return false;

	SetPassword(data->GetBool(0));
	return true;
}


/*
   ___  ____  __  _____   _      __      
  / _ \/ __ \/  |/  / /  (_)__  / /__ ___
 / // / /_/ / /|_/ / /__/ / _ \/  '_/(_-<
/____/\____/_/  /_/____/_/_//_/_/\_\/___/
*/
const std::vector<DOMAttribute> DOMLink::_description = {
	Text, Url
};

int DOMLink::Initialize(CefRefPtr<CefProcessMessage> msg)
{
	// First list element to start interpretation as this class's attributes
	int pivot = super::Initialize(msg);

	const auto args = msg->GetArgumentList();
	if (args->GetSize() < _description.size() + pivot)
	{
		LogError("DOMLink: On initialization: Object description and message size do not match!");
	}
	else
	{
		for (int i = 0; i < _description.size(); i++)
		{
			if (!Update(_description[i], args->GetValue(i + 1)))
			{
				LogError("DOMLink: Failed to assign value of type ", args->GetValue(i + 1)->GetType(),
					" to attribute ", _description[i], "!");
			}
		}
	}
	return _description.size() + pivot;
}

bool DOMLink::Update(DOMAttribute attr, CefRefPtr<CefListValue> data)
{
	switch (attr) {
		case DOMAttribute::Text:	return IPCSetText(data);
		case DOMAttribute::Url:		return IPCSetUrl(data);
	}
	return super::Update(attr, data);
}

bool DOMLink::IPCSetText(CefRefPtr<CefListValue> data)
{
	if (data->GetSize() < 1 || data->GetType(0) != CefValueType::VTYPE_STRING)
		return false;

	SetText(data->GetString(0));
	return true;
}

bool DOMLink::IPCSetUrl(CefRefPtr<CefListValue> data)
{
	if (data->GetSize() < 1 ||data->GetType(0) != CefValueType::VTYPE_STRING)
		return false;

	SetUrl(data->GetString(0));
	return true;
}

/*
   __________   _______________  ______________   ___  ____
  / __/ __/ /  / __/ ___/_  __/ / __/  _/ __/ /  / _ \/ __/
 _\ \/ _// /__/ _// /__  / /   / _/_/ // _// /__/ // /\ \  
/___/___/____/___/\___/ /_/   /_/ /___/___/____/____/___/
*/

const std::vector<DOMAttribute> DOMSelectField::_description = {
	Options
};

int DOMSelectField::Initialize(CefRefPtr<CefProcessMessage> msg)
{
	// First list element to start interpretation as this class's attributes
	int pivot = super::Initialize(msg);

	const auto args = msg->GetArgumentList();
	if (args->GetSize() < _description.size() + pivot)
	{
		LogError("DOMSelectField: On initialization: Object description and message size do not match!");
	}
	else
	{
		for (int i = 0; i < _description.size(); i++)
		{
			if (!Update(_description[i], args->GetValue(i + 1)))
			{
				LogError("DOMSelectField: Failed to assign value of type ", args->GetValue(i + 1)->GetType(),
					" to attribute ", _description[i], "!");
			}
		}
	}
	return _description.size() + pivot;
}

bool DOMSelectField::Update(DOMAttribute attr, CefRefPtr<CefListValue> data)
{
	switch (attr) {
		case DOMAttribute::Options:		return IPCSetOptions(data);
	}
	return super::Update(attr, data);
}

bool DOMSelectField::IPCSetOptions(CefRefPtr<CefListValue> data)
{
	// TODO:
	return false;
}


/*
  ____               _____           ______                   __    
 / __ \_  _____ ____/ _/ /__ _    __/ __/ /__ __ _  ___ ___  / /____
/ /_/ / |/ / -_) __/ _/ / _ \ |/|/ / _// / -_)  ' \/ -_) _ \/ __(_-<
\____/|___/\__/_/ /_//_/\___/__,__/___/_/\__/_/_/_/\__/_//_/\__/___/
*/

const std::vector<DOMAttribute> OverflowElement::_description = {
	MaxScrolling, CurrentScrolling
};

int OverflowElement::Initialize(CefRefPtr<CefProcessMessage> msg)
{
	// First list element to start interpretation as this class's attributes
	int pivot = super::Initialize(msg);

	const auto args = msg->GetArgumentList();
	if (args->GetSize() < _description.size() + pivot)
	{
		LogError("OverflowElement: On initialization: Object description and message size do not match!");
	}
	else
	{
		for (int i = 0; i < _description.size(); i++)
		{
			if (!Update(_description[i], args->GetValue(i + 1)))
			{
				LogError("OverflowElement: Failed to assign value of type ", args->GetValue(i + 1)->GetType(),
					" to attribute ", _description[i], "!");
			}
		}
	}
	return _description.size() + pivot;
}

bool OverflowElement::Update(DOMAttribute attr, CefRefPtr<CefListValue> data)
{
	switch (attr) {
	case DOMAttribute::MaxScrolling:		return IPCSetMaxScrolling(data);
	case DOMAttribute::CurrentScrolling:	return IPCSetCurrentScrolling(data);
	}
	return super::Update(attr, data);
}

bool OverflowElement::IPCSetMaxScrolling(CefRefPtr<CefListValue> data)
{
	if(data->GetSize() <= 0 || data->GetType(0) != CefValueType::VTYPE_INT || data->GetType(1) != CefValueType::VTYPE_INT)
		return false;

	SetMaxScrolling(data->GetValue(0)->GetInt(), data->GetValue(1)->GetInt());
	return true;
}

bool OverflowElement::IPCSetCurrentScrolling(CefRefPtr<CefListValue> data)
{
	if (data->GetSize() <= 0 || data->GetType(0) != CefValueType::VTYPE_INT || data->GetType(1) != CefValueType::VTYPE_INT)
		return false;

	SetCurrentScrolling(data->GetValue(0)->GetInt(), data->GetValue(1)->GetInt());
	return true;
}
