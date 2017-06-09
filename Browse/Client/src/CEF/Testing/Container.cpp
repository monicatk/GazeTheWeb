//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Mueller (muellerd@uni-koblenz.de)
//============================================================================

#include "Container.h"

V8_Container::V8_Container(std::vector<ContainerAttribute> scheme)
{
    _scheme = scheme;
}

CefRefPtr<CefV8Value> V8_Container::GetDefaultValue(Datatype type)
{
    switch (type) {
    case Datatype::DOUBLE: return CefV8Value::CreateDouble(-1); break;
    case Datatype::INTEGER: return CefV8Value::CreateInt(-1); break;
    case Datatype::STRING: return CefV8Value::CreateString(""); break;
    default: {
        //std::cout << "WARNING: Container DataType seems to be not yet defined!" << std::endl;
        return NULL;
        }
    }
}

void V8_Container::AddContainerArray(CefRefPtr<CefV8Context> context, std::string js_var_name, int arraySize)
{
    if (context->Enter())
    {
        CefRefPtr<CefV8Value> array = CefV8Value::CreateArray(arraySize);
        for (int i = 0; i < arraySize; i++)
        {
            array->SetValue(i, CreateContainerV8Object());
        }

        // Add empty array of container objects to context's global object
        context->GetGlobal()->SetValue(js_var_name, array, V8_PROPERTY_ATTRIBUTE_NONE);
        context->Exit();
    }
}

CefRefPtr<CefV8Value> V8_Container::CreateContainerV8Object()
{
	// TODO CEF UPDATE: what is second parameter?
    CefRefPtr<CefV8Value> containerObj = CefV8Value::CreateObject(NULL, NULL);

    for (int i = 0; i < (int)_scheme.size(); i++)
    {
        ContainerAttribute attr = _scheme[i];

        int arraySize = attr.GetArraySize();
        if (arraySize == 0)
        {
            // Add empty V8 value as given attribute name to V8 container object
            containerObj->SetValue(attr.GetName(), GetDefaultValue(attr.GetDatatype()), V8_PROPERTY_ATTRIBUTE_NONE);
        }
        else if (arraySize > 0)
        {
            CefRefPtr<CefV8Value> array = CefV8Value::CreateArray(arraySize),
                defaultValue = GetDefaultValue(attr.GetDatatype());
            for (int j = 0; j < arraySize; j++)
            {
                array->SetValue(j, defaultValue);
            }
            // Add empty V8 array as given attribute name to V8 container object
            containerObj->SetValue(attr.GetName(), array, V8_PROPERTY_ATTRIBUTE_NONE);
        }
        else // ERROR CASE
        {
            // DLOG(INFO) << "WARNING: scheme[" << i << "] array size < 0!";
        }
    }
    return containerObj;
}

IPC_Container::IPC_Container(std::vector<ContainerAttribute> scheme)
{
    _scheme = scheme;
}

void IPC_Container::ReadContainerObjectsAndWriteToIPCMsg(
    CefRefPtr<CefV8Context> context,
    std::string container_array_variable,
    std::vector<int> amounts,
    int64 frameID,
    CefRefPtr<CefProcessMessage> msg
    )
{
    CefRefPtr<CefListValue> args = msg->GetArgumentList();
    int index = 0;

    // Write frameID at position #0
    args->SetDouble(index++, (double) frameID);

    // Write different amount of different node types at position #1
    args->SetInt(index++, amounts.size());

    // Write amount of each node type at first positions
    for (int i = 0; i < (int)amounts.size(); i++)
    {
        args->SetInt(index++, amounts[i]);
    }

    if (context->Enter())
    {
        int all_objects_done = 0;

        for (int i = 0; i < (int)amounts.size(); i++)
        {
            // Read each container object
            for (int j = 0; j < amounts[i]; j++)
            {
                // Read every attribute and get their value
                for (int k = 0; k < (int)_scheme.size(); k++)
                {
                    // DLOG(INFO) << "Reading object #" << j << ", Attribute #" << k  << "), writing to IPC args index #" << index;

                    // Get each attribute as V8Value
                    CefRefPtr<CefV8Value> attribute = GetAttributesV8Value( // TODO: There HAS TO BE a bug concerning the object's position in the array
                                                        context,
                                                        container_array_variable,
                                                        j+all_objects_done,
                                                        _scheme[k]
                                                        );

                    // Write each attribute to IPC message
                    SetIPCArgumentsValue(
                        args,
                        index,
                        attribute,
                        _scheme[k]
                        );
                }
            }
            all_objects_done += amounts[i];
        }
        context->Exit();
    }
}

void IPC_Container::GetObjectsFromIPC(
        CefRefPtr<CefProcessMessage> msg,
        std::vector<std::function<void(std::vector<int>, std::vector<double>, std::vector<std::string>, int64 frameID, int nodeID)> > singleObjectCreationFunctions
    )
{

    CefRefPtr<CefListValue> args = msg->GetArgumentList();
    int index = 0;

    int64 frameID = (int64) args->GetDouble(index++);

    // Amount of each different object type listed
    int types = args->GetInt(index++);
    //std::cout << "types: " << types << std::endl;

    std::vector<int> amount_type;
    for (int i = 0; i < types; i++)
    {
        amount_type.push_back(args->GetInt(index++));
        //std::cout << "type[" << i << "]: " << amount_type[i] << std::endl;
    }

    for (int i = 0; i < types; i++)
    {
        // Fill attributes to vectors
        std::vector<int> integers = {};
        std::vector<double> doubles = {};
        std::vector<std::string> strings = {};

        for (int j = 0; j < amount_type[i]; j++)
        {
            // Iteration for each attribute to be read
            for (int k = 0; k < (int)_scheme.size(); k++)
            {
                //DLOG(INFO) << "Object#" << j<< "/"<<amount_type[i]<<": Reading attribute #" << k << "("<< _scheme[k].GetName() << ") starting at IPC position #" << index;
                int schemeArraySize = _scheme[k].GetArraySize();
                if (schemeArraySize == 0)
                {
                    switch (_scheme[k].GetDatatype())
                    {
                        case Datatype::INTEGER:
                        {
                            integers.push_back(args->GetInt(index++));
                            break;
                        }
                        case Datatype::DOUBLE:
                        {
                            doubles.push_back(args->GetDouble(index++));
                            break;
                        }
                        case Datatype::STRING:
                        {
                            strings.push_back(args->GetString(index++));
                            break;
                        }
                    }
                }
                else if (schemeArraySize > 0)
                {
                    switch (_scheme[k].GetDatatype())
                    {
                        // TODO: Grouping of array elements with vector in vector?

                        case Datatype::INTEGER:
                        {
                            for (int l = 0; l < _scheme[k].GetArraySize(); l++)
                            {
                                integers.push_back(args->GetInt(index++));
                            }
                            break;
                        }
                        case Datatype::DOUBLE:
                        {

                            for (int l = 0; l < _scheme[k].GetArraySize(); l++)
                            {
                                doubles.push_back(args->GetDouble(index++));
                            }
                            break;
                        }
                        case Datatype::STRING:
                        {

                            for (int l = 0; l < _scheme[k].GetArraySize(); l++)
                            {
                                strings.push_back(args->GetString(index++));
                            }

                            break;
                        }
                    }
                }
                else // ERROR CASE
                {
                    // std::cout << "WARNING: Scheme array size < 0!" << std::endl;
                }
            }
            // Call function for specific object type with read data
            singleObjectCreationFunctions[i](integers, doubles, strings, frameID, j);

            // Clear all attribute values
            integers.clear();
            doubles.clear();
            strings.clear();
        }
    }
}

CefRefPtr<CefV8Value> IPC_Container::GetAttributesV8Value(
    CefRefPtr<CefV8Context> context,
    std::string array_variable,
    int obj_index_in_array,
    ContainerAttribute attr)
{
    CefRefPtr<CefV8Value> containerObjArray = context->GetGlobal()->GetValue(array_variable);

    if (obj_index_in_array < containerObjArray->GetArrayLength())
    {
        CefRefPtr<CefV8Value> containerObj = containerObjArray->GetValue(obj_index_in_array);

        // Get objects attribute by attribute's name
        return containerObj->GetValue(attr.GetName());
    }
    else
    {
        // DLOG(WARNING) << "WARNING: V8ArrayLength not as big as expected!";
        // TODO: Handle NULL in following steps...
        return CefV8Value::CreateNull();
    }
}

void IPC_Container::SetIPCArgumentsValue(
    CefRefPtr<CefListValue> args,
    int& position,
    CefRefPtr<CefV8Value> v8Value,
    ContainerAttribute attr)
{

    if (attr.GetArraySize() == 0)
    {
        switch (attr.GetDatatype())
        {
            case Datatype::INTEGER:
            {
                if (v8Value->IsInt())
                {
                    args->SetInt(position++, v8Value->GetIntValue());
                }
                else
                {
                    //DLOG(WARNING) << "ERROR(SetIPCArgumentsValue): Said \"int\", got something else at position=" << position++;
                }
                break;
            }

            case Datatype::DOUBLE:
            {
                if (v8Value->IsDouble())
                {
                    args->SetDouble(position++, v8Value->GetDoubleValue());
                }
                else
                {
                    //DLOG(WARNING) << "ERROR(SetIPCArgumentsValue): Said \"double\", got something else at position=" << position++;
                }
                break;
            }

            case Datatype::STRING:
            {
                if (v8Value->IsString())
                {
                    args->SetString(position++, v8Value->GetStringValue());
                }
                else
                {
                    //DLOG(WARNING) << "ERROR(SetIPCArgumentsValue): Said \"string\", got something else at position=" << position++;
                }
                break;
            }
        }
    }
    else if (attr.GetArraySize() > 0) // V8Value is an array
    {
        for (int i = 0; i < attr.GetArraySize(); i++)
        {
            switch (attr.GetDatatype())
            {
            case Datatype::INTEGER:
            {
                if (v8Value->GetValue(i)->IsInt())
                {
                    args->SetInt(position++, v8Value->GetValue(i)->GetIntValue());
                }
                else
                {
                    //DLOG(WARNING) << "ERROR(SetIPCArgumentsValue): Said \"int\", got something else at position=" << position++;
                }
                break;
            }

            case Datatype::DOUBLE:
            {
                if (v8Value->GetValue(i)->IsDouble())
                {
                    args->SetDouble(position++, v8Value->GetValue(i)->GetDoubleValue());
                }
                else
                {
                    //DLOG(WARNING) << "ERROR(SetIPCArgumentsValue): Said \"double\", got something else at position=" << position++;
                }
                break;
            }

            case Datatype::STRING:
            {
                if (v8Value->GetValue(i)->IsString())
                {
                    args->SetString(position++, v8Value->GetValue(i)->GetStringValue());
                }
                else
                {
                    //DLOG(WARNING) << "ERROR(SetIPCArgumentsValue): Said \"string\", got something else at position=" << position++;
                }
                break;
            }
            }
        }
    }
    else // ERROR CASE
    {
        //DLOG(WARNING) << "ERROR(SetIPCArgumentsValue): Array size < 0!";
        position++;	// Go on with next element
    }
}
