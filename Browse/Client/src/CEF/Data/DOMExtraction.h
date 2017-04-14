//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Mueller (muellerd@uni-koblenz.de)
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#ifndef DOMEXTRACTION_H_
#define DOMEXTRACTION_H_

#include <src/CEF/Data/DOMAttribute.h>
#include <src/Utils/Logger.h>
#include <include/cef_values.h>
#include <include/cef_v8.h>
#include <map>
#include <functional>
#include <algorithm>



/** Do only use if V8 context was entered before! */
namespace V8ToCefListValue
{
	// Nested lists
	const CefRefPtr<CefListValue> NestedListOfDoubles(CefRefPtr<CefV8Value> v8rects);

	// Lists
	const CefRefPtr<CefListValue> ListOfStrings(CefRefPtr<CefV8Value> attrData);
	const CefRefPtr<CefListValue> ListOfIntegers(CefRefPtr<CefV8Value> attrData);

	// Primitive types - TODO: There certainly exists a more generic approach for each primitive type!
	const CefRefPtr<CefListValue> Boolean(CefRefPtr<CefV8Value> attrData);
	const CefRefPtr<CefListValue> Integer(CefRefPtr<CefV8Value> attrData);
	const CefRefPtr<CefListValue> String(CefRefPtr<CefV8Value> attrData);

	const std::map<const DOMAttribute, const std::string> AttrGetter = {
		{ DOMAttribute::Rects,				"getRects" },
		{ DOMAttribute::FixedId,			"getFixedId" },
		{ DOMAttribute::OverflowId,			"getOverflowId" },
		{ DOMAttribute::Text,				"getText" },
		{ DOMAttribute::IsPassword,			"getIsPassword" },
		{ DOMAttribute::Url,				"getUrl" },
		{ DOMAttribute::Options,			"getOptions" },
		{ DOMAttribute::MaxScrolling,		"getMaxScrolling"},
		{ DOMAttribute::CurrentScrolling,	"getCurrentScrolling"}

	};

	const std::map < const DOMAttribute, const std::function<CefRefPtr<CefListValue>(CefRefPtr<CefV8Value>)> > AttrConversion = {
		{ DOMAttribute::Rects,				&NestedListOfDoubles },
		{ DOMAttribute::FixedId,			&Integer },
		{ DOMAttribute::OverflowId,			&Integer },
		{ DOMAttribute::Text,				&String },
		{ DOMAttribute::IsPassword,			&Boolean },
		{ DOMAttribute::Url,				&String },
		{ DOMAttribute::Options,			&ListOfStrings },
		{ DOMAttribute::MaxScrolling,		&ListOfIntegers },
		{ DOMAttribute::CurrentScrolling,	&ListOfIntegers }
	
	};

	// NOTE: Debug output won't reach the user's console, because this code is run in the Renderer process
	const static CefRefPtr<CefListValue> ExtractAttributeData(DOMAttribute attr, CefRefPtr<CefV8Value> obj)
	{
		if (obj->IsNull() || obj->IsUndefined())
			return CefRefPtr<CefListValue>();

		// Early exit
		if (AttrGetter.find(attr) == AttrGetter.end() || AttrConversion.find(attr) == AttrConversion.end())
		{
			LogError("V8ToCefListValue conversion: Insufficient information defined for DOMAttribute ", (int)attr, "! Abort.");
			return CefRefPtr<CefListValue>();
		}

		// Return getter function,
		CefRefPtr<CefV8Value> getter = obj->GetValue(AttrGetter.at(attr));

		// Check if getter function is valid
		if (getter->IsUndefined() || getter->IsNull() || !getter->IsFunction())
		{
			LogBug("V8ToCefListValue conversion: Could not access getter function for DOMAttribute ", (int)attr, " in Javascript object.");
			return CefRefPtr<CefListValue>();
		}

		// Execute getter function
		CefRefPtr<CefV8Value> data = getter->ExecuteFunction(obj, {});

		if (data->IsUndefined() || data->IsNull())
			return CefRefPtr<CefListValue>();

		// Convert returned V8Value to CefListValue
		return AttrConversion.at(attr)(data);
	}
}


namespace StringToCefListValue
{
	// Nested lists
	const CefRefPtr<CefListValue> NestedListOfDoubles(std::string attrData);

	// Lists
	const CefRefPtr<CefListValue> ListOfStrings(std::string attrData);
	const CefRefPtr<CefListValue> ListOfIntegers(std::string attrData);

	// Primitive types
	const CefRefPtr<CefListValue> Boolean(std::string attrData);
	const CefRefPtr<CefListValue> Integer(std::string attrData);
	const CefRefPtr<CefListValue> String(std::string attrData);

	const std::map<const DOMAttribute, const std::function<CefRefPtr<CefListValue>(std::string)> > AttrConversion =
	{
		{DOMAttribute::Rects,				&NestedListOfDoubles},
		{DOMAttribute::FixedId,				&Integer},
		{DOMAttribute::OverflowId,			&Integer},
		{DOMAttribute::Text,				&String},
		{DOMAttribute::IsPassword,			&Boolean},
		{DOMAttribute::Url,					&String},
		{DOMAttribute::Options,				&ListOfStrings},
		{DOMAttribute::MaxScrolling,		&ListOfIntegers},
		{DOMAttribute::CurrentScrolling,	&ListOfIntegers}	
	};


	const static CefRefPtr<CefListValue> ExtractAttributeData(DOMAttribute attr, std::string attrData)
	{
		// Early exit
		if (AttrConversion.find(attr) == AttrConversion.end())
		{
			LogError("StringToCefListValue conversion: Insufficient information defined for DOMAttribute ", (int)attr, "! Abort.");
			return CefRefPtr<CefListValue>();
		}

		return AttrConversion.at(attr)(attrData);
	}
}

#endif // DOMEXTRACTION_H_