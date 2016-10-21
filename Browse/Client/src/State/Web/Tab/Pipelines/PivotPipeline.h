//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Pipeline for pivot usage.

#ifndef PIVOTPIPELINE_H_
#define PIVOTPIPELINE_H_

#include "Pipeline.h"

class PivotPipeline : public Pipeline
{
public:

    // Constructor
    PivotPipeline(TabInteractionInterface* pTab);
};

#endif // PIVOTPIPELINE_H_
