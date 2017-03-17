//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Mueller (muellerd@uni-koblenz.de)
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "src/State/Web/Tab/Tab.h"
#include "src/Master.h"
#include "src/Setup.h"
#include "src/Utils/QuadRenderItem.h"
#include "src/Utils/Helper.h"
#include "src/Utils/Logger.h"
#include "submodules/glm/glm/gtc/matrix_transform.hpp"

// Shader programs (For debugging rectangles)
const std::string vertexShaderSource =
"#version 330 core\n"
"in vec3 posAttr;\n"
"in vec2 uvAttr;\n"
"out vec2 uv;\n"
"uniform mat4 matrix;\n"
"void main() {\n"
"   uv = uvAttr;\n"
"   gl_Position = matrix * vec4(posAttr, 1);\n"
"}\n";

const std::string fragmentShaderSource =
"#version 330 core\n"
"in vec2 uv;\n"
"out vec4 fragColor;\n"
"uniform vec3 color;\n"
"void main() {\n"
"   fragColor = vec4(color,1);\n"
"}\n";

void Tab::InitDebuggingOverlay()
{
	// Line quad
	_upDebugLineQuad = std::unique_ptr<RenderItem>(
		new QuadRenderItem(
			vertexShaderSource,
			fragmentShaderSource,
			Quad::Type::LINES_WITH_DIAGONAL));

	// Fill quad
	_upDebugFillQuad = std::unique_ptr<RenderItem>(
		new QuadRenderItem(
			vertexShaderSource,
			fragmentShaderSource,
			Quad::Type::TRIANGLES));
}

void Tab::DrawDebuggingOverlay() const
{
	// Reserve variables
	glm::mat4 model, matrix;

	// Projection
	glm::mat4 projection = glm::ortho(0, 1, 0, 1);

	// Do only draw following when no pipeline is active
	if (!_pipelineActive)
	{
		// Define render function (should not be defined each call)
		const std::function<void(Rect, bool)> renderRect = [&](Rect rect, bool fixed)
		{
			// Fixed or not
			if (!fixed)
			{
				// Subtract scrolling while coordinates are in CEFPixel space
				rect.left -= _scrollingOffsetX;
				rect.right -= _scrollingOffsetX;
				rect.bottom -= _scrollingOffsetY;
				rect.top -= _scrollingOffsetY;
			}

			// Scale from CEFPixel space to WebViewPixel
			rect.left = (rect.left / (float)_upWebView->GetResolutionX()) * (float)_upWebView->GetWidth();
			rect.right = (rect.right / (float)_upWebView->GetResolutionX()) * (float)_upWebView->GetWidth();
			rect.bottom = (rect.bottom / (float)_upWebView->GetResolutionY()) * (float)_upWebView->GetHeight();
			rect.top = (rect.top / (float)_upWebView->GetResolutionY()) * (float)_upWebView->GetHeight();

			// Calculate model matrix
			model = glm::mat4(1.0f);
			model = glm::scale(model, glm::vec3(1.f / _pMaster->GetWindowWidth(), 1.f / _pMaster->GetWindowHeight(), 1.f));
			model = glm::translate(model, glm::vec3(_upWebView->GetX() + rect.left, _upWebView->GetHeight() - (rect.bottom), 1));
			model = glm::scale(model, glm::vec3(rect.Width(), rect.Height(), 0));

			// Combine matrics
			matrix = projection * model;

			// Fill uniform with matrix (no need for Bind() since bound in called context)
			_upDebugLineQuad->GetShader()->UpdateValue("matrix", matrix);

			// Render rectangle
			_upDebugLineQuad->Draw(GL_LINES);
		};

		// Bind debug render item for line quad
		_upDebugLineQuad->Bind();

		// ### DOMTRIGGER ###

		// Set rendering up for DOMTrigger
		_upDebugLineQuad->GetShader()->UpdateValue("color", DOM_TRIGGER_DEBUG_COLOR);

		// Go over all DOMTriggers
		for (const auto& rDOMTrigger : _DOMTriggers)
		{
			// Render rects
			for (const auto rRect : rDOMTrigger->GetDOMRects())
			{
				if (true)//rDOMTrigger->GetDOMVisibility())
				{
					if (!rDOMTrigger->GetDOMIsPasswordField())
					{
						renderRect(rRect, rDOMTrigger->GetDOMFixed());
					}
					else
					{
						_upDebugLineQuad->GetShader()->UpdateValue("color", glm::vec3(0.0f, 1.f, 1.f));
						renderRect(rRect, rDOMTrigger->GetDOMFixed());
						_upDebugLineQuad->GetShader()->UpdateValue("color", DOM_TRIGGER_DEBUG_COLOR);
					}
				}

			}
		}

		// ### DOMTEXTLINKS ###

		// Set rendering up for DOMTextLink
		_upDebugLineQuad->GetShader()->UpdateValue("color", DOM_TEXT_LINKS_DEBUG_COLOR);

		// Go over all DOMTextLinks
		for (const auto& rDOMTextLink : _DOMTextLinks)
		{
			// Render rects
			for (const auto rRect : rDOMTextLink->GetRects())
			{
				if (rDOMTextLink->GetVisibility())
					renderRect(rRect, rDOMTextLink->GetFixed());
				else
				{
					_upDebugLineQuad->GetShader()->UpdateValue("color", glm::vec3(1.0f, 0.f, 1.f));
					renderRect(rRect, rDOMTextLink->GetFixed());
					_upDebugLineQuad->GetShader()->UpdateValue("color", DOM_TEXT_LINKS_DEBUG_COLOR);
				}
			}
		}

		// DEBUG - links containing line break are shown in another color
		_upDebugLineQuad->GetShader()->UpdateValue("color", glm::vec3(1.f, 0.f, 1.f));
		for (const auto& rDOMTextLink : _DOMTextLinks)
		{
			if (rDOMTextLink->GetRects().size() == 2 && rDOMTextLink->GetVisibility())
				renderRect(rDOMTextLink->GetRects()[1], rDOMTextLink->GetFixed());
		}

		// ### SELECT FIELDS ###
		// Set rendering up for DOMSelectFields
		_upDebugLineQuad->GetShader()->UpdateValue("color", DOM_SELECT_FIELD_DEBUG_COLOR);
		for (const auto& rDOMSelectField : _DOMSelectFields)
		{
			// Render rects
			for (const auto rRect : rDOMSelectField->GetRects())
			{
				renderRect(rRect, rDOMSelectField->GetFixed());
			}
		}

		// ### FIXED ELEMENTS ###

		// Set rendering up for fixed element
		_upDebugLineQuad->GetShader()->UpdateValue("color", FIXED_ELEMENT_DEBUG_COLOR);

		// Go over all fixed elements vectors
		for (const auto& rFixedElements : _fixedElements)
		{
			// Go over fixed elements
			for (const auto& rFixedElement : rFixedElements)
			{
				// Render rect
				if (rFixedElement.Height() > 0 && rFixedElement.Width() > 0)
					renderRect(rFixedElement, true);
			}
		}

		// ### OVERFLOW ELEMENTS ###
		_upDebugLineQuad->GetShader()->UpdateValue("color", glm::vec3(255.f / 255.f, 127.f / 255.f, 35.f / 255.f));

		for (const auto& rOverflowElement : _overflowElements)
		{
			if (rOverflowElement) // Note: Can be NULL if aquivalent element in JS got deleted. (see Tab::RemoveOverflowElement)
			{
				for (const auto& rect : rOverflowElement->GetRects())
					renderRect(rect, rOverflowElement->GetFixed());
			}

		}
	}

	// ### EXACT GAZE INPUT VISUALIZTION ###

	// Bind render item and set color
	_upDebugFillQuad->Bind();
	_upDebugFillQuad->GetShader()->UpdateValue("color", glm::vec3(255.f / 255.f, 127.f / 255.f, 35.f / 255.f));

	// Do it for each gaze sample
	for (const glm::vec2& rGaze : _gazeDebuggingQueue)
	{
		// Calculate model matrix
		model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(1.f / _pMaster->GetWindowWidth(), 1.f / _pMaster->GetWindowHeight(), 1.f));
		model = glm::translate(model, glm::vec3(rGaze.x, _pMaster->GetWindowHeight() - rGaze.y, 1));
		model = glm::scale(model, glm::vec3(2, 2, 0));

		// Combine matrics
		matrix = projection * model;

		// Fill uniform with matrix
		_upDebugFillQuad->GetShader()->UpdateValue("matrix", matrix);

		// Render rectangle
		_upDebugFillQuad->Draw(GL_TRIANGLES);
	}
}
