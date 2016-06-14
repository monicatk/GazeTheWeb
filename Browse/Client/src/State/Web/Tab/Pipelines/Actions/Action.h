//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Abstract superclass of all actions. Action is atomic interaction of user with
// current tab.

#ifndef ACTION_H_
#define ACTION_H_

#include "src/Utils/TabInput.h"
#include "src/State/Web/Tab/Pipelines/Actions/ActionDataMap.h"

// Forward declaration
class TabInteractionInterface;

class Action
{
public:

    // Constructor
    Action(TabInteractionInterface* pTab);

    // Destructor
    virtual ~Action() = 0;

    // Update retuns whether finished with execution
    virtual bool Update(float tpf, TabInput tabInput) = 0;

    // Draw
    virtual void Draw() const = 0;

    // Activate
    virtual void Activate() = 0;

    // Deactivate
    virtual void Deactivate() = 0;

    // Abort
    virtual void Abort() = 0;

    // Set input data value
    void SetInputValue(std::string type, int value);
    void SetInputValue(std::string type, int64 value);
    void SetInputValue(std::string type, float value);
    void SetInputValue(std::string type, glm::vec2 value);
    void SetInputValue(std::string type, std::string value);
    void SetInputValue(std::string type, std::u16string value);

    // Get output data value in reference and returns, whether value was filled
    bool GetOutputValue(std::string type, int& rValue) const;
    bool GetOutputValue(std::string type, int64& rValue) const;
    bool GetOutputValue(std::string type, float& rValue) const;
    bool GetOutputValue(std::string type, glm::vec2& rValue) const;
    bool GetOutputValue(std::string type, std::string& rValue) const;
    bool GetOutputValue(std::string type, std::u16string& rValue) const;

protected:

    // Add slot to data maps
    void AddIntInputSlot(std::string type);
    void AddInt64InputSlot(std::string type);
    void AddFloatInputSlot(std::string type);
    void AddVec2InputSlot(std::string type);
    void AddStringInputSlot(std::string type);
    void AddString16InputSlot(std::string type);

    void AddIntOutputSlot(std::string type);
    void AddInt64OutputSlot(std::string type);
    void AddFloatOutputSlot(std::string type);
    void AddVec2OutputSlot(std::string type);
    void AddStringOutputSlot(std::string type);
    void AddString16OutputSlot(std::string type);

    // Get input data value in reference and returns, whether value was filled
    bool GetInputValue(std::string type, int& rValue) const;
    bool GetInputValue(std::string type, int64& rValue) const;
    bool GetInputValue(std::string type, float& rValue) const;
    bool GetInputValue(std::string type, glm::vec2& rValue) const;
    bool GetInputValue(std::string type, std::string& rValue) const;
    bool GetInputValue(std::string type, std::u16string& rValue) const;

    // Set output data value
    void SetOutputValue(std::string type, int value);
    void SetOutputValue(std::string type, int64 value);
    void SetOutputValue(std::string type, float value);
    void SetOutputValue(std::string type, glm::vec2 value);
    void SetOutputValue(std::string type, std::string);
    void SetOutputValue(std::string type, std::u16string);

    // Pointer to interface which enables interaction with tab
    TabInteractionInterface* _pTab;

private:

    // Input data
    ActionDataMap _inputData;

    // Output data
    ActionDataMap _outputData;

};

#endif // ACTION_H_
