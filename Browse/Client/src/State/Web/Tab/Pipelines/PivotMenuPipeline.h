//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Pipeline for pivot menu.

#ifndef PIVOTMENUPIPELINE_H_
#define PIVOTMENUPIPELINE_H_

#include "Pipeline.h"

class PivotMenuPipeline : public Pipeline
{
public:

    // Constructor
	PivotMenuPipeline(TabInteractionInterface* pTab);
};

#endif // PIVOTMENUPIPELINE_H_
