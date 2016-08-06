//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Mueller (muellerd@uni-koblenz.de)
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "src/State/Web/Tab/Tab.h"
#include "src/CEF/Extension/CefMediator.h"
#include "src/Utils/Texture.h"

void Tab::PushBackPipeline(std::unique_ptr<Pipeline> upPipeline)
{
	_pipelines.push_back(std::move(upPipeline));
	if (!_pipelineActive)
	{
		// Activate first pipeline
		_pipelines.front()->Activate();

		// Remember that there is active pipeline and activate GUI to abort it
		SetPipelineActivity(true);
	}
}

void Tab::EmulateLeftMouseButtonClick(double x, double y, bool visualize)
{
	// Tell mediator about the click
	_pCefMediator->EmulateLeftMouseButtonClick(this, x, y);

	// Add some visualization for the user
	if (visualize)
	{
		AddClickVisualization(x, y);
	}
}

void Tab::EmulateMouseWheelScrolling(double deltaX, double deltaY)
{
	_pCefMediator->EmulateMouseWheelScrolling(this, deltaX, deltaY);
}

void Tab::InputTextData(int64 frameID, int nodeID, std::string text, bool submit)
{
	_pCefMediator->InputTextData(this, frameID, nodeID, text, submit);
}

void Tab::GetWebViewTextureResolution(int& rWidth, int& rHeight) const
{
	if (auto spTexture = _upWebView->GetTexture().lock())
	{
		rWidth = spTexture->GetWidth();
		rHeight = spTexture->GetHeight();
	}
	else
	{
		rWidth = 0;
		rHeight = 0;
	}
}