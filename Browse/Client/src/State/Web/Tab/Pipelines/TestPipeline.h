//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Pipeline for testing new actions.

#ifndef TESTPIPELINE_H_
#define TESTPIPELINE_H_

#include "Pipeline.h"

class TestPipeline : public Pipeline
{
public:

    // Constructor
    TestPipeline(TabInteractionInterface* pTab);
};

#endif // TESTPIPELINE_H_
