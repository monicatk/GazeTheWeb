//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Mueller (muellerd@uni-koblenz.de)
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "DOMExtraction.h"
#include <src/Utils/Helper.h>



const CefRefPtr<CefListValue> V8ToCefListValue::NestedListOfDoubles(CefRefPtr<CefV8Value> v8rects)
{
	if (!v8rects->IsArray())
		return CefRefPtr<CefListValue>();

	auto rects = CefListValue::Create();
	for (int i = 0; i < v8rects->GetArrayLength(); i++)
	{
		CefRefPtr<CefV8Value> v8rect = v8rects->GetValue(i);

		if (!v8rect->IsArray())
			return CefRefPtr<CefListValue>();

		CefRefPtr<CefListValue> rect = CefListValue::Create();
		for (int j = 0; j < v8rect->GetArrayLength(); j++)
		{
			rect->SetDouble(j, v8rect->GetValue(j)->GetDoubleValue());
		}
		rects->SetList(i, rect);
	}
	return rects;
}

const CefRefPtr<CefListValue> V8ToCefListValue::ListOfStrings(CefRefPtr<CefV8Value> attrData)
{
	if (!attrData->IsArray())
		return CefRefPtr<CefListValue>();

	CefRefPtr<CefListValue> list = CefListValue::Create();
	for (int i = 0; i < attrData->GetArrayLength(); i++)
	{
		CefRefPtr<CefValue> val = CefValue::Create();

		if (!attrData->GetValue(i)->IsString())
			continue;

		val->SetString(attrData->GetValue(i)->GetStringValue());
		list->SetValue(i, val);
	}

	return list;
}

const CefRefPtr<CefListValue> V8ToCefListValue::ListOfIntegers(CefRefPtr<CefV8Value> attrData)
{
	if (!attrData->IsArray())
		return CefRefPtr<CefListValue>();

	CefRefPtr<CefListValue> list = CefListValue::Create();
	for (int i = 0; i < attrData->GetArrayLength(); i++)
	{
		CefRefPtr<CefValue> val = CefValue::Create();

		if (!attrData->GetValue(i)->IsInt())
			continue;

		val->SetInt(attrData->GetValue(i)->GetIntValue());
		list->SetValue(i, val);
	}

	return list;
}

const CefRefPtr<CefListValue> V8ToCefListValue::Boolean(CefRefPtr<CefV8Value> attrData)
{
	if (!attrData->IsBool())
		return CefRefPtr<CefListValue>();

	CefRefPtr<CefListValue> list = CefListValue::Create();
	CefRefPtr<CefValue> val = CefValue::Create();
	val->SetInt(attrData->GetBoolValue());
	list->SetValue(0, val);
	return list;
}

const CefRefPtr<CefListValue> V8ToCefListValue::Integer(CefRefPtr<CefV8Value> attrData)
{
	if (!attrData->IsInt())
		return CefRefPtr<CefListValue>();

	CefRefPtr<CefListValue> list = CefListValue::Create();
	CefRefPtr<CefValue> id = CefValue::Create();
	id->SetInt(attrData->GetIntValue());
	list->SetValue(0, id);
	return list;
}

const CefRefPtr<CefListValue> V8ToCefListValue::String(CefRefPtr<CefV8Value> attrData)
{
	if (!attrData->IsString())
		return CefRefPtr<CefListValue>();

	CefRefPtr<CefListValue> list = CefListValue::Create();
	CefRefPtr<CefValue> val = CefValue::Create();
	val->SetInt(attrData->GetIntValue());
	list->SetValue(0, val);
	return list;
}

const CefRefPtr<CefListValue> StringToCefListValue::NestedListOfDoubles(std::string attrData)
{
	CefRefPtr<CefListValue> extracted_data = CefValue::Create();
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

		extracted_data->SetValue(i / 4, rect);
	}

	return extracted_data;
}

const CefRefPtr<CefListValue> StringToCefListValue::ListOfStrings(std::string attrData)
{
	CefRefPtr<CefListValue> extracted_data = CefValue::Create();

	std::vector<std::string> options = SplitBySeparator(attrData, ';');
	for (int i = 0; i < options.size(); i++)
	{
		CefRefPtr<CefValue> option = CefValue::Create();
		option->SetString(options[i]);
		extracted_data->SetValue(i, option);
	}

	return extracted_data;
}

const CefRefPtr<CefListValue> StringToCefListValue::ListOfIntegers(std::string attrData)
{
	CefRefPtr<CefListValue> extracted_data = CefValue::Create();

	std::vector<std::string> options = SplitBySeparator(attrData, ';');
	for (int i = 0; i < options.size(); i++)
	{
		CefRefPtr<CefValue> option = CefValue::Create();
		option->SetInt(std::stoi(options[i]));
		extracted_data->SetValue(i, option);
	}

	return extracted_data;
}

const CefRefPtr<CefListValue> StringToCefListValue::Boolean(std::string attrData)
{
	CefRefPtr<CefListValue> extracted_data = CefValue::Create();

	CefRefPtr<CefValue> val = CefValue::Create();
	val->SetBool(std::stoi(attrData));
	extracted_data->SetValue(0, val);

	return extracted_data;
}

const CefRefPtr<CefListValue> StringToCefListValue::Integer(std::string attrData)
{
	CefRefPtr<CefListValue> extracted_data = CefValue::Create();

	CefRefPtr<CefValue> val = CefValue::Create();
	val->SetInt(std::stoi(attrData));
	extracted_data->SetValue(0, val);

	return extracted_data;
}

const CefRefPtr<CefListValue> StringToCefListValue::String(std::string attrData)
{
	CefRefPtr<CefListValue> extracted_data = CefValue::Create();

	CefRefPtr<CefValue> val = CefValue::Create();
	val->SetString(attrData);
	extracted_data->SetValue(0, val);

	return extracted_data;
}
