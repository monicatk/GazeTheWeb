//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Template for all data that can be exchanged between actions.

#ifndef ACTIONDATA_H_
#define ACTIONDATA_H_

#include <string>

template <class T>
class ActionData
{
public:

    // Constructor
    ActionData() {}

    // Set value data (and remembers, that value was actively set)
    void SetValue(T value) { _value = value; _filled = true; }

    // Get value
    T GetValue() const { return _value; }

    // Is filled
    bool IsFilled() const { return _filled; }

private:

    // Value of data
    T _value;

    // Bool for indication whether was filled
    bool _filled = false;

};

#endif // ACTIONDATA_H_
