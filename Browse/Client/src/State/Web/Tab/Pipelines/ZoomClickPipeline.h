//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Pipeline to emulate clicking by zooming to a screen coordinate controlled
// with gaze.

#ifndef ZOOMCLICKPIPELINE_H_
#define ZOOMCLICKPIPELINE_H_

#include "Pipeline.h"

class ZoomClickPipeline : public Pipeline
{
public:

    // Constructor
    ZoomClickPipeline(TabInteractionInterface* pTab);
};

#endif // ZOOMCLICKPIPELINE_H_
