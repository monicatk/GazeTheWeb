//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Mueller (muellerd@uni-koblenz.de)
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#ifndef DOMEXTRACTION_H_
#define DOMEXTRACTION_H_

#include <include/cef_values.h>
#include <include/cef_v8.h>
#include <map>
#include <functional>

enum DOMAttribute {
	Rects = 0, FixedId, OverflowId,
	Text, IsPassword, Url, Options
};

const std::map<const DOMAttribute, const std::string> attrGetter = {
	{DOMAttribute::Rects,		"getRects"},
	{DOMAttribute::FixedId,		"getFixedId"},
	{DOMAttribute::OverflowId,	"getOverflowId"},
	{DOMAttribute::Text,		"getText"},
	{DOMAttribute::IsPassword,	"getPassword"},
	{DOMAttribute::Url,			"getUrl"},
	{DOMAttribute::Options,		"getOptions"}

};

CefRefPtr<CefListValue> extractNestedListOfDoubles(CefRefPtr<CefV8Value> v8rects)
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

const std::map < const DOMAttribute, const std::function<CefRefPtr<CefListValue>(CefRefPtr<CefV8Value>)> > v8ToListValue = {
	// RECTS
	{DOMAttribute::Rects, &extractNestedListOfDoubles},
	// FIXEDID
	{DOMAttribute::FixedId, [](const CefRefPtr<CefV8Value> v8id) {
			if (!v8id->IsInt())
				return CefRefPtr<CefListValue>();

			CefRefPtr<CefListValue> list = CefListValue::Create();
			auto id = CefValue::Create();
			id->SetInt(v8id->GetIntValue());
			list->SetValue(0, id);
			return list;
		}
	}
};


const static CefRefPtr<CefListValue> ExtractAttributeDataFromString(DOMAttribute attr, std::string attrData)
{
	CefRefPtr<CefListValue> extracted_data = CefValue::Create();

	switch (attr)
	{
	case(DOMAttribute::Rects):
	{
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
		break;
	};
	case(DOMAttribute::FixedId):
	case(DOMAttribute::OverflowId):
	{
		CefRefPtr<CefValue> val = CefValue::Create();
		val->SetInt(std::stoi(attrData));
		extracted_data->SetValue(0, val);
		break;
	};
	case(DOMAttribute::Text):
	case(DOMAttribute::Url):
	{
		CefRefPtr<CefValue> val = CefValue::Create();
		val->SetString(attrData);
		extracted_data->SetValue(0, val);
		break;
	}
	case(DOMAttribute::IsPassword):
	{
		CefRefPtr<CefValue> val = CefValue::Create();
		val->SetBool(std::stoi(attrData));
		extracted_data->SetValue(0, val);
		break;
	}
	case(DOMAttribute::Options):
	{
		std::vector<std::string> options = SplitBySeparator(attrData, ';');
		for (int i = 0; i < options.size(); i++)
		{
			CefRefPtr<CefValue> option = CefValue::Create();
			option->SetString(options[i]);
			extracted_data->SetValue(i, option);
		}
		break;
	}
	}
	return extracted_data;
}

const static CefRefPtr<CefListValue> ExtractAttributeDataFromV8Object(DOMAttribute attr, CefRefPtr<CefV8Value> obj)
{
	// Return getter function, if known and listed in map, otherwise return NULL
	CefRefPtr<CefV8Value> getter = (attrGetter.find(attr) != attrGetter.end()) ? 
		obj->GetValue(attrGetter.at(attr)) : CefV8Value::CreateNull();	

	// Check if getter function is valid
	if (!getter->IsFunction())
		return CefV8Value::CreateNull();

	// Execute getter function
	CefRefPtr<CefV8Value> data = getter->ExecuteFunction(obj, {});

	// Convert returned V8Value to CefListValue
}

#endif // DOMEXTRACTION_H_