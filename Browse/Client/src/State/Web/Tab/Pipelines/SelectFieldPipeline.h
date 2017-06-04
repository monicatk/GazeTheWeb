//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Pipeline to interact with select fields on web pages.

#ifndef SELECTFIELDPIPELINE_H_
#define SELECTFIELDPIPELINE_H_

#include "Pipeline.h"
#include "src/CEF/Data/DOMNode.h"

class SelectFieldPipeline : public Pipeline
{
public:

    // Constructor
	SelectFieldPipeline(TabInteractionInterface* pTab, std::shared_ptr<DOMSelectField> spNode);

};

#endif // SELECTFIELDPIPELINE_H_
