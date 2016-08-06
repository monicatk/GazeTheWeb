//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Input structure for tab which holds coordinates in web view space.
// Origin in upper left corner.

#ifndef TABINPUT_H_
#define TABINPUT_H_

#include "src/Utils/Input.h"

class TabInput : public Input
{
public:
    TabInput(
        int gazeX,
        int gazeY,
        bool gazeUsed,
        int webViewGazeX,
        int webViewGazeY,
        float webViewGazeRelativeX,
        float webViewGazeRelativeY) : Input(gazeX, gazeY, gazeUsed)
    {
        this->webViewGazeX = webViewGazeX;
        this->webViewGazeY = webViewGazeY;
        this->webViewGazeRelativeX = webViewGazeRelativeX;
        this->webViewGazeRelativeY = webViewGazeRelativeY;
        this->gazeUsed = gazeUsed;
        insideWebView =
            this->webViewGazeRelativeX < 1.f
            && this->webViewGazeRelativeX >=0
            && this->webViewGazeRelativeY < 1.f
            && this->webViewGazeRelativeY >= 0;
    }

    int webViewGazeX;
    int webViewGazeY;
    float webViewGazeRelativeX;
    float webViewGazeRelativeY;
    bool insideWebView;
};

#endif // TABINPUT_H_
