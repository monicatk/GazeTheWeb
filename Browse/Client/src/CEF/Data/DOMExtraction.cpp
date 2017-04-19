//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Mueller (muellerd@uni-koblenz.de)
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "DOMExtraction.h"
#include <src/Utils/Helper.h>



const CefRefPtr<CefListValue> V8ToCefListValue::NestedListOfDoubles(CefRefPtr<CefV8Value> v8rects)
{
	if (v8rects->IsUndefined() || v8rects->IsNull() || !v8rects->IsArray())
		return CefRefPtr<CefListValue>();

	CefRefPtr<CefListValue> wrapper = CefListValue::Create();
	CefRefPtr<CefListValue> rects = CefListValue::Create();
	for (int i = 0; i < v8rects->GetArrayLength(); i++)
	{
		CefRefPtr<CefV8Value> v8rect = v8rects->GetValue(i);
		if (v8rect->IsUndefined() || v8rect->IsNull() || !v8rect->IsArray())
			return CefRefPtr<CefListValue>();
		
		CefRefPtr<CefListValue> rect = CefListValue::Create();
		for (int j = 0; j < v8rect->GetArrayLength(); j++)
		{
			rect->SetDouble(j, v8rect->GetValue(j)->GetDoubleValue());
		}
		rects->SetList(i, rect);
	}

	wrapper->SetList(0, rects);
	return wrapper;
}

const CefRefPtr<CefListValue> V8ToCefListValue::ListOfStrings(CefRefPtr<CefV8Value> attrData)
{
	if (!attrData->IsArray())
		return CefRefPtr<CefListValue>();

	CefRefPtr<CefListValue> wrapper = CefListValue::Create();
	CefRefPtr<CefListValue> list = CefListValue::Create();
	for (int i = 0; i < attrData->GetArrayLength(); i++)
	{
		if (!attrData->GetValue(i)->IsString())
			list->SetString(i, "");
		else 
			list->SetString(i, attrData->GetValue(i)->GetStringValue());
	}
	wrapper->SetList(0, list);
	return wrapper;
}

const CefRefPtr<CefListValue> V8ToCefListValue::ListOfIntegers(CefRefPtr<CefV8Value> attrData)
{
	if (!attrData->IsArray())
		return CefRefPtr<CefListValue>();

	CefRefPtr<CefListValue> wrapper = CefListValue::Create();
	CefRefPtr<CefListValue> list = CefListValue::Create();
	for (int i = 0; i < attrData->GetArrayLength(); i++)
	{
		if (!attrData->GetValue(i)->IsInt())
			list->SetInt(i, -1);
		else
			list->SetInt(i, attrData->GetValue(i)->GetIntValue());
	}
	wrapper->SetList(0, list);
	return wrapper;
}

const CefRefPtr<CefListValue> V8ToCefListValue::Boolean(CefRefPtr<CefV8Value> attrData)
{
	if (!attrData->IsBool())
		return CefRefPtr<CefListValue>();

	CefRefPtr<CefListValue> wrapper = CefListValue::Create();
	wrapper->SetBool(0, attrData->GetBoolValue());
	return wrapper;
}

const CefRefPtr<CefListValue> V8ToCefListValue::Integer(CefRefPtr<CefV8Value> attrData)
{
	if (attrData->IsNull() || attrData->IsUndefined() || !attrData->IsInt()) // TODO: Check everywhere for null and undefined!
		return CefRefPtr<CefListValue>();

	CefRefPtr<CefListValue> wrapper = CefListValue::Create();
	wrapper->SetInt(0, attrData->GetIntValue());
	return wrapper;
}

const CefRefPtr<CefListValue> V8ToCefListValue::String(CefRefPtr<CefV8Value> attrData)
{
	if (!attrData->IsString())
		return CefRefPtr<CefListValue>();

	CefRefPtr<CefListValue> wrapper = CefListValue::Create();
	wrapper->SetString(0, attrData->GetStringValue());
	return wrapper;
}









const CefRefPtr<CefListValue> StringToCefListValue::NestedListOfDoubles(std::string attrData)
{
	CefRefPtr<CefListValue> wrapper = CefListValue::Create();
	CefRefPtr<CefListValue> extracted_data = CefListValue::Create();
	std::vector<std::string> rectDataStr = SplitBySeparator(attrData, ';');
	std::vector<double> rectData;

	// Extract each float value from string, earlier separated by ';', and convert string to numerical value
	std::for_each(
		rectDataStr.begin(),
		rectDataStr.end(),
		[&rectData](std::string value) {rectData.push_back(std::stod(value)); }
	);

	// Read out each 4 float values und create 1 Rect with them
	for (int i = 0; i + 3 < rectData.size(); i += 4)
	{
		CefRefPtr<CefListValue> rect = CefListValue::Create();
		for (int j = 0; j < 4; j++)
		{
			CefRefPtr<CefValue> val = CefValue::Create();
			val->SetDouble(rectData[i + j]);
			rect->SetValue(j, val);
		}

		extracted_data->SetList(i / 4, rect);
	}
	wrapper->SetList(0, extracted_data);
	return wrapper;
}

const CefRefPtr<CefListValue> StringToCefListValue::ListOfStrings(std::string attrData)
{
	CefRefPtr<CefListValue> wrapper = CefListValue::Create();
	CefRefPtr<CefListValue> extracted_data = CefListValue::Create();

	std::vector<std::string> options = SplitBySeparator(attrData, ';');
	for (int i = 0; i < options.size(); i++)
	{
		extracted_data->SetString(i, options[i]);
	}
	wrapper->SetList(0, extracted_data);
	return wrapper;
}

const CefRefPtr<CefListValue> StringToCefListValue::ListOfIntegers(std::string attrData)
{
	CefRefPtr<CefListValue> wrapper = CefListValue::Create();
	CefRefPtr<CefListValue> extracted_data = CefListValue::Create();

	std::vector<std::string> options = SplitBySeparator(attrData, ';');
	for (int i = 0; i < options.size(); i++)
	{
		extracted_data->SetInt(i, std::stoi(options[i]));
	}
	wrapper->SetList(0, extracted_data);
	return wrapper;
}

const CefRefPtr<CefListValue> StringToCefListValue::Boolean(std::string attrData)
{
	CefRefPtr<CefListValue> wrapper = CefListValue::Create();
	wrapper->SetBool(0, std::stoi(attrData));
	return wrapper;
}

const CefRefPtr<CefListValue> StringToCefListValue::Integer(std::string attrData)
{
	CefRefPtr<CefListValue> wrapper = CefListValue::Create();
	wrapper->SetInt(0, std::stoi(attrData));
	return wrapper;
}

const CefRefPtr<CefListValue> StringToCefListValue::String(std::string attrData)
{
	CefRefPtr<CefListValue> wrapper = CefListValue::Create();
	wrapper->SetString(0, attrData);
	return wrapper;
}
