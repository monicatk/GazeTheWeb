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
        PushBackClickVisualization(x, y);
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

std::weak_ptr<const DOMNode> Tab::GetNearestLink(glm::vec2 pageCoordinate, float& rDistance) const
{
    if(_DOMTextLinks.empty())
    {
        // No link available
        rDistance = -1;
        return std::weak_ptr<DOMNode>();
    }
    else
    {
        // Get link with minimal distance
        float minDistance = std::numeric_limits<float>::max();
        std::weak_ptr<const DOMNode> wpResult;

        // Go over links
        for(const auto& rLink : _DOMTextLinks)
        {
            // Go over rectangles of that link
            for(const auto& rRect : rLink->GetRects())
            {
                glm::vec2 center = rRect.center();
                float width = rRect.width();
                float height = rRect.height();

                // Distance
                float dx = glm::max(glm::abs(pageCoordinate.x - center.x) - (width / 2.f), 0.f);
                float dy = glm::max(glm::abs(pageCoordinate.y - center.y) - (height / 2.f), 0.f);
                float distance = glm::sqrt((dx * dx) + (dy * dy));

                // Check whether distance is smaller
                if(distance < minDistance)
                {
                    minDistance = distance;
                    wpResult = rLink;
                }
            }
        }

        // Return result
        rDistance = minDistance;
        return wpResult;
    }

}

void  Tab::ScrollOverflowElement(int elemId, int x, int y)
{
	_pCefMediator->ScrollOverflowElement(this, elemId, x, y);
}
