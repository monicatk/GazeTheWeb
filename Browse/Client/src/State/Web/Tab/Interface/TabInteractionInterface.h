//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Tab interface for interaction.

#ifndef TABINTERACTIONINTERFACE_H_
#define TABINTERACTIONINTERFACE_H_

#include "src/State/Web/Tab/Interface/TabActionInterface.h"
#include "src/State/Web/Tab/Interface/TabOverlayInterface.h"
#include "src/State/Web/Tab/Interface/TabDebuggingInterface.h"

// Combination of Action and Overlay interface since both have different functionalities but are used in same classes

class TabInteractionInterface: public TabActionInterface, public TabOverlayInterface, public TabDebuggingInterface
{};

#endif // TABINTERACTIONINTERFACE_H_
