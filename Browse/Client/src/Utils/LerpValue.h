//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Float value which can be easily linear interpolated.

#ifndef LERP_VALUE_H_
#define LERP_VALUE_H_

#include "submodules/glm/glm/glm.hpp"

class LerpValue
{
public:

    // Constructors
    LerpValue(float value, float min, float max)
    {
        _min = min;
        _max = max;
        setValue(value);
    }
    LerpValue(float value) : LerpValue(value, 0, 1) {}
    LerpValue() : LerpValue(0, 0, 1) {}

    // Destructor
    virtual ~LerpValue() {}

    // Update, returns updated raw value
    float update(float delta)
    {
        _value += delta;
        _value = glm::clamp(_value, _min, _max);
        return _value;
    }

    // Update, returns updated raw value
    float update(float delta, bool subtract)
    {
        if (subtract)
        {
            return update(-delta);
        }
        else
        {
            return update(delta);
        }
    }

    // Getter for value
    float getValue() const
    {
        return _value;
    }

    // Setter for value
    void setValue(float value)
    {
        _value = value;
        _value = glm::clamp(_value, _min, _max);
    }

    // Setter for min value
    void setMin(float min)
    {
        _min = min;
        _value = glm::clamp(_value, _min, _max);
    }

    // Setter for max value
    void setMax(float max)
    {
        _max = max;
        _value = glm::clamp(_value, _min, _max);
    }

    // Getter for min value
    float getMin() const
    {
        return _min;
    }

    // Getter for max value
    float getMax() const
    {
        return _max;
    }

    // Assignment
    LerpValue& operator=(LerpValue other)
    {
        this->_value = other.getValue();
        this->_max = other.getMax();
        this->_min = other.getMin();
        return *this;
    }

private:

    // Members
    float _value;
    float _min;
    float _max;
};

#endif // LERP_VALUE_H_
