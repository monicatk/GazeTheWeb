//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Input struct. Origin in upper left display corner.

#ifndef INPUT_H_
#define INPUT_H_

class Input
{
public:

    Input(int gazeX, int gazeY, bool gazeUsed, bool instantInteraction, bool saccade)
    {
        this->gazeX = gazeX;
        this->gazeY = gazeY;
        this->gazeUsed = gazeUsed;
		this->instantInteraction = instantInteraction;
		this->saccade = saccade;
    }

    int gazeX;
    int gazeY;
    bool gazeUsed;
	bool instantInteraction;
	bool saccade; // indicator whether current gaze is classified as part of a saccade
};

#endif // INPUT_H_
