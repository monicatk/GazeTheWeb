//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Pipeline to input text on web pages.

#ifndef TEXTINPUTPIPELINE_H_
#define TEXTINPUTPIPELINE_H_

#include "Pipeline.h"
#include "src/CEF/Data/DOMNode.h"

class TextInputPipeline : public Pipeline
{
public:

    // Constructor
    TextInputPipeline(TabInteractionInterface* pTab, std::shared_ptr<DOMTextInput> spNode);

};

#endif // TEXTINPUTPIPELINE_H_
