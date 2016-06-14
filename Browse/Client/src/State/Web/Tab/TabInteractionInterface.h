//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Tab interface for interaction.

#ifndef TABINTERACTIONINTERFACE_H_
#define TABINTERACTIONINTERFACE_H_

#include "src/State/Web/Tab/TabActionInterface.h"
#include "src/State/Web/Tab/TabOverlayInterface.h"

// Combination of Action and Overlay interface since both have different functionalities but are used in same classes

class TabInteractionInterface: public TabActionInterface, public TabOverlayInterface
{
public:

    // Calculate position and size of web view
    virtual void CalculateWebViewPositionAndSize(int& rX, int& rY, int& rWidth, int& rHeight) const = 0;

    // Getter for window with and height
    virtual void GetWindowSize(int& rWidth, int& rHeight) const = 0;
};

#endif // TABINTERACTIONINTERFACE_H_
