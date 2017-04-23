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
	// Check if there are enough arguments in msg to initialize each attribute
	if ((int) _description.size() > args->GetSize() - 1)
	{
		LogError("DOMNode: On initialization: Object description (", _description.size(), ") and message (", args->GetSize(), 
			") size do not match!");
	}
	else
	{
		for (unsigned int i = 0; i < _description.size(); i++)
		{
			CefRefPtr<CefListValue> data = args->GetList(i + 1);	
			if (!Update(_description[i], data))
			{
				LogError("DOMNode: Failed to assign value of type ", data->GetType(0),
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

	if (data->GetSize() > 0  && data->GetValue(0)->GetType() != CefValueType::VTYPE_LIST)
		return false;

	const auto rectList = data->GetList(0);
	std::vector<Rect> rects;
	//LogDebug("IPCSetRects: #Rects: ", rectList->GetSize());
	for (int i = 0; i < rectList->GetSize(); i++)
	{
		const auto rectData = rectList->GetList(i);
		std::vector<float> rect;
		for (int j = 0; rectData && j < rectData->GetSize(); j++)
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
	{
		LogDebug("Expected CefValueType: ", CefValueType::VTYPE_INT, ", but got: ", data->GetType(0));
		return false;
	}

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
	// Check if there are enough arguments in msg to initialize each attribute
	if ((int)_description.size() > (int) args->GetSize() - pivot)
	{
		LogError("DOMTextInput: On initialization: Object description and message size do not match!");
	}
	else
	{
		for (unsigned int i = 0; i < _description.size(); i++)
		{
			CefRefPtr<CefListValue> data = args->GetList(pivot + i);

			if (!Update(_description[i], data))
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
	if (data->GetSize() < 1 || data->GetValue(0)->GetType() != CefValueType::VTYPE_STRING)
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
	// Check if there are enough arguments in msg to initialize each attribute
	if ((int)_description.size() > (int) args->GetSize() - pivot)
	{
		LogError("DOMLink: On initialization: Object description ", _description.size(), " + " , pivot, " and message size ", args->GetSize() ," do not match!");
	}
	else
	{
		for (unsigned int i = 0; i < _description.size(); i++)
		{
			CefRefPtr<CefListValue> data = args->GetList(pivot + i);	// Access argument list, where super class has finished

			if (!Update(_description[i], data))
			{
				LogError("DOMLink: Failed to assign value of type ", data->GetValue(0)->GetType(), // TODO: Could this crash the renderer process if data is a CefListValue?
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
	if (data->GetSize() < 1 || data->GetValue(0)->GetType() != CefValueType::VTYPE_STRING)
	{
		LogDebug("data->type: ", data->GetValue(0)->GetType());
		LogDebug("deeper type: ", data->GetList(0)->GetType(0));
		return false;
	}

	SetText(data->GetString(0));
	return true;
}

bool DOMLink::IPCSetUrl(CefRefPtr<CefListValue> data)
{
	if (data->GetSize() < 1 || data->GetValue(0)->GetType() != CefValueType::VTYPE_STRING)
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
	// Check if there are enough arguments in msg to initialize each attribute
	if ((int)_description.size() > (int) args->GetSize() - pivot)
	{
		LogError("DOMSelectField: On initialization: Object description and message size do not match!");
	}
	else
	{
		for (unsigned int i = 0; i < _description.size(); i++)
		{
			CefRefPtr<CefListValue> data = args->GetList(pivot + i);

			if (!Update(_description[i], data))
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

const std::vector<DOMAttribute> DOMOverflowElement::_description = {
	MaxScrolling, CurrentScrolling
};

int DOMOverflowElement::Initialize(CefRefPtr<CefProcessMessage> msg)
{

	// First list element to start interpretation as this class's attributes
	int pivot = super::Initialize(msg);

	const auto args = msg->GetArgumentList();
	// Check if there are enough arguments in msg to initialize each attribute
	if ((int)_description.size() > (int) args->GetSize() - pivot)
	{
		LogError("OverflowElement: On initialization: Object description and message size do not match!");
	}
	else
	{
		for (unsigned int i = 0; i < _description.size(); i++)
		{
			CefRefPtr<CefListValue> data = args->GetList(pivot + i);

			if (!Update(_description[i], data))
			{
				LogError("OverflowElement: Failed to assign value of type ", data->GetType(0),
					" to attribute ", _description[i], "!");
			}
		}
	}


	return _description.size() + pivot;
}

bool DOMOverflowElement::Update(DOMAttribute attr, CefRefPtr<CefListValue> data)
{
	switch (attr) {
	case DOMAttribute::MaxScrolling:		return IPCSetMaxScrolling(data);
	case DOMAttribute::CurrentScrolling:	return IPCSetCurrentScrolling(data);
	}
	return super::Update(attr, data);
}

bool DOMOverflowElement::IPCSetMaxScrolling(CefRefPtr<CefListValue> data)
{
	if (data->GetSize() < 1)
		return false;

	CefRefPtr<CefListValue> list = data->GetList(0);
	if (list->GetSize() < 2 || list->GetType(0) != CefValueType::VTYPE_INT || list->GetType(1) != CefValueType::VTYPE_INT)
		return false;

	SetMaxScrolling(list->GetInt(0), list->GetInt(1));
	return true;
}

bool DOMOverflowElement::IPCSetCurrentScrolling(CefRefPtr<CefListValue> data)
{
	if (data->GetSize() < 1) 
		return false;

	CefRefPtr<CefListValue> list = data->GetList(0);
	if (list->GetSize() < 2 || list->GetType(0) != CefValueType::VTYPE_INT || list->GetType(1) != CefValueType::VTYPE_INT)
		return false;

	SetCurrentScrolling(list->GetInt(0), list->GetInt(1));
	return true;
}
void DOM::GetJSRepresentation(
	std::string nodeType,
	std::vector<const std::vector<DOMAttribute>*>& description,
	std::string & obj_getter_name)
{
	if (nodeType == "TextInputData")
	{
		DOMTextInput::GetDescription(&description);
		obj_getter_name = DOMTextInput::GetJSObjectGetter();
		return;

	}
	if (nodeType == "LinkData")
	{
		DOMLink::GetDescription(&description);
		obj_getter_name = DOMLink::GetJSObjectGetter();
		return;

	}
	if (nodeType == "SelectFieldData")
	{
		DOMSelectField::GetDescription(&description);
		obj_getter_name = DOMSelectField::GetJSObjectGetter();
		return;

	}
	if (nodeType == "OverflowElementData")
	{
		DOMOverflowElement::GetDescription(&description);
		obj_getter_name = DOMOverflowElement::GetJSObjectGetter(); 
		return;
	}
}
