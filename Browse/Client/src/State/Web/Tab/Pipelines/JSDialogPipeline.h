//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Pipeline handling JavaScript callbacks.

#ifndef JSDIALOGPIPELINE_H_
#define JSDIALOGPIPELINE_H_

#include "Pipeline.h"
#include "src/CEF/JavaScriptDialogType.h"

class JSDialogPipeline : public Pipeline
{
public:

	// Constructor
	JSDialogPipeline(TabInteractionInterface* pTab, JavaScriptDialogType type, std::string message);
};

#endif // JSDIALOGPIPELINE_H_
