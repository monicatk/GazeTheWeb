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
#include <map>

class ActionDataMap
{
public:

    // Add slot to map
    void AddIntSlot(std::string type);
    void AddInt64Slot(std::string type);
    void AddFloatSlot(std::string type);
    void AddVec2Slot(std::string type);
    void AddStringSlot(std::string type);
    void AddString16Slot(std::string type);

    // Set value of data
    void SetValue(std::string type, int value);
    void SetValue(std::string type, int64 value);
    void SetValue(std::string type, float value);
    void SetValue(std::string type, glm::vec2 value);
    void SetValue(std::string type, std::string value);
    void SetValue(std::string type, std::u16string value);

    // Fills value into given reference variable. Returns whether value was filled
    bool GetValue(std::string type, int& rValue) const;
    bool GetValue(std::string type, int64& rValue) const;
    bool GetValue(std::string type, float& rValue) const;
    bool GetValue(std::string type, glm::vec2& rValue) const;
    bool GetValue(std::string type, std::string& rValue) const;
    bool GetValue(std::string type, std::u16string& rValue) const;

private:

    // Maps with data
    std::map<std::string, ActionData<int> > _intMap;
    std::map<std::string, ActionData<int64> > _int64Map;
    std::map<std::string, ActionData<float> > _floatMap;
    std::map<std::string, ActionData<glm::vec2> > _vec2Map;
    std::map<std::string, ActionData<std::string> > _stringMap;
    std::map<std::string, ActionData<std::u16string> > _string16Map;
};

#endif // ACTIONDATAMAP_H_
