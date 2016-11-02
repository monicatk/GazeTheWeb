//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Pipeline for simple left button mouse click.

#ifndef LEFTMOUSEBUTTONCLICKPIPELINE_H_
#define LEFTMOUSEBUTTONCLICKPIPELINE_H_

#include "Pipeline.h"

class LeftMouseButtonClickPipeline : public Pipeline
{
public:

    // Constructor. Taking click coordiante in screen space as input
	LeftMouseButtonClickPipeline(TabInteractionInterface* pTab, glm::vec2 coordinate);
};

#endif // LEFTMOUSEBUTTONCLICKPIPELINE_H_
