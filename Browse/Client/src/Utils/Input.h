//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Input struct.

#ifndef INPUT_H_
#define INPUT_H_

class Input
{
public:

    Input(int gazeX, int gazeY, bool gazeUsed)
    {
        this->gazeX = gazeX;
        this->gazeY = gazeY;
        this->gazeUsed = gazeUsed;
    }

    int gazeX;
    int gazeY;
    bool gazeUsed;
};

#endif // INPUT_H_
