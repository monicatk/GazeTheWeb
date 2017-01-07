//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Müller (muellerd@uni-koblenz.de)
//============================================================================

#ifndef CEF_CONTAINER_H_
#define CEF_CONTAINER_H_

#include "include/cef_v8.h"
#include <functional>

enum Datatype
{
    INTEGER, DOUBLE, STRING
};

class ContainerAttribute
{
public:
    ContainerAttribute(std::string name, Datatype type, int arraysize)
    {
        _name = name;
        _type = type;
        _arraysize = arraysize;
    }

    std::string GetName() const { return _name; }
    Datatype GetDatatype() const { return _type; }
    int GetArraySize() const { return _arraysize; }

private:
    std::string _name;
    Datatype _type;
    int _arraysize;
};

/* DEFINE COMMON SCHEMES HERE */

// Mirror definition of Javascript objects used to read out DOM node information
const std::vector<ContainerAttribute> domNodeScheme = {
    ContainerAttribute("coordinates", Datatype::DOUBLE, 4),
    ContainerAttribute("value", Datatype::STRING, 0)
};

/* END OF SCHEME DEFINITIONS */

class V8_Container
{
public:
    V8_Container(std::vector<ContainerAttribute> scheme);

    // Add array to global object of given context
    void AddContainerArray(CefRefPtr<CefV8Context> context, std::string js_var_name, int arraySize);

private:
    // Used in CreateContainerArray
    CefRefPtr<CefV8Value> CreateContainerV8Object();

    CefRefPtr<CefV8Value> GetDefaultValue(Datatype type);

    // Defines attributes and their data types
    std::vector<ContainerAttribute> _scheme;
};

class IPC_Container
{
public:
    IPC_Container(std::vector<ContainerAttribute> scheme);

    // Write objects to IPC message's argument list, use |amounts| as amount[i] separator of different node types
    void ReadContainerObjectsAndWriteToIPCMsg(
        CefRefPtr<CefV8Context> context,
        std::string js_var_name,
        std::vector<int> amounts,
        int64 frameID,
        CefRefPtr<CefProcessMessage> msg);

    void GetObjectsFromIPC(
        CefRefPtr<CefProcessMessage> msg,
        std::vector
        <
            std::function
            <
                void (std::vector<int>, std::vector<double>, std::vector<std::string>, int64 frameID, int nodeID)
            >
        > singleObjectCreationFunctions);

private:
    CefRefPtr<CefV8Value> GetAttributesV8Value(
        CefRefPtr<CefV8Context> context,
        std::string array_variable,
        int obj_index_in_array,
        ContainerAttribute attr);

    void SetIPCArgumentsValue(
        CefRefPtr<CefListValue> args,
        int& position,					// |&| because arrays increase index by more than 1!
        CefRefPtr<CefV8Value> v8Value,
        ContainerAttribute attr);

    std::vector<ContainerAttribute> _scheme;
};

#endif // CEF_CONTAINER_H_
