//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Pipeline for simple left button double mouse click.

#ifndef LEFTMOUSEBUTTONDOUBLECLICKPIPELINE_H_
#define LEFTMOUSEBUTTONDOUBLECLICKPIPELINE_H_

#include "Pipeline.h"

class LeftMouseButtonDoubleClickPipeline : public Pipeline
{
public:

    // Constructor. Taking click coordiante in WebViewPixel space as input
	LeftMouseButtonDoubleClickPipeline(TabInteractionInterface* pTab, glm::vec2 coordinate);
};

#endif // LEFTMOUSEBUTTONDOUBLECLICKPIPELINE_H_
