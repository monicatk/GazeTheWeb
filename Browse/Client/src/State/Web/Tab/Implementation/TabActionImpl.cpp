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

void Tab::EmulateLeftMouseButtonClick(double x, double y, bool visualize, bool isScreenCoordinate)
{
	// Add some visualization for the user at screen position
	if (visualize)
	{
		// Maybe convert into screen coordinate system
		double screenX = x;
		double screenY = y;
		if (!isScreenCoordinate)
		{
			screenX = (screenX / (float)_upWebView->GetResolutionX()) * (float)_upWebView->GetWidth();
			screenY = (screenY / (float)_upWebView->GetResolutionY()) * (float)_upWebView->GetHeight();
		}
		PushBackClickVisualization(screenX, screenY);
	}

	// Convert screen to render pixel coordinate
	if (isScreenCoordinate)
	{
		x = (x / (float)_upWebView->GetWidth()) * (float)_upWebView->GetResolutionX();
		y = (y / (float)_upWebView->GetHeight()) * (float)_upWebView->GetResolutionY();
	}

	// Tell mediator about the click
	_pCefMediator->EmulateLeftMouseButtonClick(this, x, y);
}

void Tab::EmulateMouseCursor(double x, double y, bool leftButtonPressed, bool isScreenCoordinate)
{
	// Convert screen to render pixel coordinate
	if (isScreenCoordinate)
	{
		x = (x / (float)_upWebView->GetWidth()) * (float)_upWebView->GetResolutionX();
		y = (y / (float)_upWebView->GetHeight()) * (float)_upWebView->GetResolutionY();
	}

	// Tell mediator about the cursor
	_pCefMediator->EmulateMouseCursor(this, x, y, leftButtonPressed);
}

void Tab::EmulateMouseWheelScrolling(double deltaX, double deltaY)
{
	_pCefMediator->EmulateMouseWheelScrolling(this, deltaX, deltaY);
}

void Tab::EmulateLeftMouseButtonDown(double x, double y, bool isScreenCoordinate)
{
	if (isScreenCoordinate)
	{
		x = (x / (float)_upWebView->GetWidth()) * (float)_upWebView->GetResolutionX();
		y = (y / (float)_upWebView->GetHeight()) * (float)_upWebView->GetResolutionY();
	}

	// Tell mediator about mouse button down
	_pCefMediator->EmulateLeftMouseButtonDown(this, x, y);
}

void Tab::EmulateLeftMouseButtonUp(double x, double y, bool isScreenCoordinate)
{
	if (isScreenCoordinate)
	{
		x = (x / (float)_upWebView->GetWidth()) * (float)_upWebView->GetResolutionX();
		y = (y / (float)_upWebView->GetHeight()) * (float)_upWebView->GetResolutionY();
	}

	// Tell mediator about mouse button up
	_pCefMediator->EmulateLeftMouseButtonUp(this, x, y);
}

void Tab::InputTextData(int64 frameID, int nodeID, std::string text, bool submit)
{
	_pCefMediator->InputTextData(this, frameID, nodeID, text, submit);
}

std::weak_ptr<const DOMNode> Tab::GetNearestLink(glm::vec2 screenCoordinate, float& rDistance) const
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
				center.x = (center.x / (float)_upWebView->GetResolutionX()) * (float)_upWebView->GetWidth();
				center.y = (center.y / (float)_upWebView->GetResolutionY()) * (float)_upWebView->GetHeight();
                float width = (rRect.width() / (float)_upWebView->GetResolutionX()) * (float)_upWebView->GetWidth();
                float height = (rRect.height() / (float)_upWebView->GetResolutionY()) * (float)_upWebView->GetHeight();

                // Distance
                float dx = glm::max(glm::abs(screenCoordinate.x - center.x) - (width / 2.f), 0.f);
                float dy = glm::max(glm::abs(screenCoordinate.y - center.y) - (height / 2.f), 0.f);
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
