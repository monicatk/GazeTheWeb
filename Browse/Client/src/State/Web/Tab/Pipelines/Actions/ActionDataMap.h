//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Map of string, defining type of data or semantic of data, and C++ data type.
// Every action has one ActionDataMap as input and one as output.

#ifndef ACTIONDATAMAP_H_
#define ACTIONDATAMAP_H_

#include "src/State/Web/Tab/Pipelines/Actions/ActionData.h"
#include "src/Typedefs.h"
#include "src/Utils/glmWrapper.h"
#include "src/Utils/Logger.h"
#include <map>
#include <memory>

class ActionDataMap
{
public:

    // Add slot to map
    void AddIntSlot(std::string type, int value = 0);
    void AddInt64Slot(std::string type, int64 value = 0);
	void AddFloatSlot(std::string type, float value = 0.f);
    void AddVec2Slot(std::string type, glm::vec2 value = glm::vec2(0.f, 0.f));
    void AddStringSlot(std::string type, std::string value = "");
    void AddString16Slot(std::string type, std::u16string value = u"");

    // Set value of data
    void SetValue(std::string type, int value)				{ SetValue<int>(type, value, _intMap); }
    void SetValue(std::string type, int64 value)			{ SetValue<int64>(type, value, _int64Map); }
    void SetValue(std::string type, float value)			{ SetValue<float>(type, value, _floatMap); }
    void SetValue(std::string type, glm::vec2 value)		{ SetValue<glm::vec2>(type, value, _vec2Map); }
    void SetValue(std::string type, std::string value)		{ SetValue<std::string>(type, value, _stringMap); }
    void SetValue(std::string type, std::u16string value)	{ SetValue<std::u16string>(type, value, _string16Map); }

    // Fills value into given reference variable. Returns whether value was filled actively
    bool GetValue(std::string type, int& rValue) const				{ return GetValue<int>(type, rValue, _intMap); }
    bool GetValue(std::string type, int64& rValue) const			{ return GetValue<int64>(type, rValue, _int64Map); }
    bool GetValue(std::string type, float& rValue) const			{ return GetValue<float>(type, rValue, _floatMap); }
    bool GetValue(std::string type, glm::vec2& rValue) const		{ return GetValue<glm::vec2>(type, rValue, _vec2Map); }
    bool GetValue(std::string type, std::string& rValue) const		{ return GetValue<std::string>(type, rValue, _stringMap); }
	bool GetValue(std::string type, std::u16string& rValue) const	{ return GetValue<std::u16string>(type, rValue, _string16Map); }

private:

	// Type definition of data map
	template <typename T>
	using InternalDataMap = std::map<std::string, std::unique_ptr<ActionData< T > > >;

	// Set value template method
	template <typename T>
	void SetValue(std::string type, T value, InternalDataMap<T>& rMap)
	{
		auto iter = rMap.find(type);
		if (iter != rMap.end())
		{
			iter->second->SetValue(value);
		}
		else
		{
			LogBug("No slot in ActionDataMap for ", type);
		}
	}

	// Get value template method. Returns whether value was filled actively
	template <typename T>
	bool GetValue(std::string type, T& rValue, const InternalDataMap<T>& rMap) const
	{
		auto iter = rMap.find(type);
		if (iter != rMap.end())
		{
			rValue = iter->second->GetValue();
			return iter->second->IsFilled();
		}
		else
		{
			LogBug("No slot in ActionDataMap for ", type);
		}
		return false;
	}

    // Maps with data
	InternalDataMap<int>			_intMap;
	InternalDataMap<int64>			_int64Map;
	InternalDataMap<float>			_floatMap;
	InternalDataMap<glm::vec2>		_vec2Map;
	InternalDataMap<std::string>	_stringMap;
	InternalDataMap<std::u16string>	_string16Map;
};

#endif // ACTIONDATAMAP_H_
