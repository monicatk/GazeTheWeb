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
#include "submodules/glm/glm/gtc/matrix_transform.hpp" // TODO: move to debug rendering class
#include <algorithm>

// Shaders (For debugging rectangles)
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

Tab::Tab(Master* pMaster, CefMediator* pCefMediator, WebTabInterface* pWeb, std::string url)
{
	// Fill members
	_pMaster = pMaster;
	_pCefMediator = pCefMediator;
	_pWeb = pWeb;
	_url = url;

	// Create layouts for Tab (overlay at first, because behind other layouts)
	_pOverlayLayout = _pMaster->AddLayout("layouts/Overlay.xeyegui", EYEGUI_TAB_LAYER, false);
	_pScrollingOverlayLayout = _pMaster->AddLayout("layouts/Overlay.xeyegui", EYEGUI_TAB_LAYER, false);
	_pPanelLayout = _pMaster->AddLayout("layouts/Tab.xeyegui", EYEGUI_TAB_LAYER, false);
	_pPipelineAbortLayout = _pMaster->AddLayout("layouts/TabPipelineAbort.xeyegui", EYEGUI_TAB_LAYER, false);

	// Create scroll up and down floating frames in special overlay layout which always on top of standard overlay
	_scrollUpProgressFrameIndex = eyegui::addFloatingFrameWithBrick(
		_pScrollingOverlayLayout,
		"bricks/TabScrollUpProgress.beyegui",
		0.5f - (TAB_SCROLLING_SENSOR_WIDTH / 2.f),
		TAB_SCROLLING_SENSOR_PADDING,
		TAB_SCROLLING_SENSOR_WIDTH,
		TAB_SCROLLING_SENSOR_HEIGHT,
		false,
		false);
	_scrollDownProgressFrameIndex = eyegui::addFloatingFrameWithBrick(
		_pScrollingOverlayLayout,
		"bricks/TabScrollDownProgress.beyegui",
		0.5f - (TAB_SCROLLING_SENSOR_WIDTH / 2.f),
		1.f - TAB_SCROLLING_SENSOR_PADDING - TAB_SCROLLING_SENSOR_HEIGHT,
		TAB_SCROLLING_SENSOR_WIDTH,
		TAB_SCROLLING_SENSOR_HEIGHT,
		false,
		false);
	_scrollUpSensorFrameIndex = eyegui::addFloatingFrameWithBrick(
		_pScrollingOverlayLayout,
		"bricks/TabScrollUpSensor.beyegui",
		0.5f - (TAB_SCROLLING_SENSOR_WIDTH / 2.f),
		TAB_SCROLLING_SENSOR_PADDING,
		TAB_SCROLLING_SENSOR_WIDTH,
		TAB_SCROLLING_SENSOR_HEIGHT,
		false,
		false);
	_scrollDownSensorFrameIndex = eyegui::addFloatingFrameWithBrick(
		_pScrollingOverlayLayout,
		"bricks/TabScrollDownSensor.beyegui",
		0.5f - (TAB_SCROLLING_SENSOR_WIDTH / 2.f),
		1.f - TAB_SCROLLING_SENSOR_PADDING - TAB_SCROLLING_SENSOR_HEIGHT,
		TAB_SCROLLING_SENSOR_WIDTH,
		TAB_SCROLLING_SENSOR_HEIGHT,
		false,
		false);

	// Register listener
	_spTabButtonListener = std::shared_ptr<TabButtonListener>(new TabButtonListener(this));
	_spTabSensorListener = std::shared_ptr<TabSensorListener>(new TabSensorListener(this));
	eyegui::registerButtonListener(_pPanelLayout, "click_mode", _spTabButtonListener);
	eyegui::registerButtonListener(_pPanelLayout, "auto_scrolling", _spTabButtonListener);
	eyegui::registerButtonListener(_pPanelLayout, "scroll_to_top", _spTabButtonListener);
	eyegui::registerButtonListener(_pPanelLayout, "zoom", _spTabButtonListener);
	eyegui::registerButtonListener(_pPipelineAbortLayout, "abort", _spTabButtonListener);
	eyegui::registerSensorListener(_pScrollingOverlayLayout, "scroll_up_sensor", _spTabSensorListener);
	eyegui::registerSensorListener(_pScrollingOverlayLayout, "scroll_down_sensor", _spTabSensorListener);

	// Create listener for overlay
	_spTabOverlayButtonListener = std::shared_ptr<TabOverlayButtonListener>(new TabOverlayButtonListener(this));
	_spTabOverlayKeyboardListener = std::shared_ptr<TabOverlayKeyboardListener>(new TabOverlayKeyboardListener(this));
	_spTabOverlayWordSuggestListener = std::shared_ptr<TabOverlayWordSuggestListener>(new TabOverlayWordSuggestListener(this));

	// Create WebView owned by Tab
	int webRenderWidth, webRenderHeight = 0;
	GetWebRenderResolution(webRenderWidth, webRenderHeight);
	_upWebView = std::unique_ptr<WebView>(new WebView(webRenderWidth, webRenderHeight));

	// Activate current mode
	ActivateMode(_mode);

	// Register itself and painted texture in mediator to receive DOMNodes
	_pCefMediator->RegisterTab(this);

	// Prepare debug rendering
	_upDebugRenderItem = std::unique_ptr<RenderItem>(new QuadRenderItem(vertexShaderSource, fragmentShaderSource));
}

Tab::~Tab()
{
	// Delete triggers before removing layout
	_DOMTriggers.clear();

	_pCefMediator->UnregisterTab(this);
	_pMaster->RemoveLayout(_pPanelLayout);
	_pMaster->RemoveLayout(_pPipelineAbortLayout);
	_pMaster->RemoveLayout(_pOverlayLayout);
	_pMaster->RemoveLayout(_pScrollingOverlayLayout);
}

void Tab::Update(float tpf, Input& rInput)
{
	// ### UPDATE WEB VIEW ###

	// Update WebView (get values from eyeGUI layout directly)
	auto webViewPositionAndSize = CalculateWebViewPositionAndSize();
	_upWebView->Update(
		webViewPositionAndSize.x,
		webViewPositionAndSize.y,
		webViewPositionAndSize.width,
		webViewPositionAndSize.height);

	// ### UPDATE OVERLAY ###

	// Update click visualization
	for (int i = 0; i < (int)_clickVisualizations.size(); i++)
	{
		// Decrement visibility
		_clickVisualizations.at(i).fading -= tpf;

		// If fading done, remove from layout
		if (_clickVisualizations.at(i).fading <= 0)
		{
			eyegui::removeFloatingFrame(_pOverlayLayout, _clickVisualizations.at(i).frameIndex, false);
		}
		else
		{
			// Fade it by making it smaller
			float relativeSize = (_clickVisualizations.at(i).fading / CLICK_VISUALIZATION_DURATION) * CLICK_VISUALIZATION_RELATIVE_SIZE;

			// Position of floating frame
			float relativePositionX = (webViewPositionAndSize.x + _clickVisualizations.at(i).x) / (float)_pMaster->GetWindowWidth();
			float relativePositionY = (webViewPositionAndSize.y + _clickVisualizations.at(i).y) / (float)_pMaster->GetWindowHeight();
			relativePositionX -= relativeSize / 2.f;
			relativePositionY -= relativeSize / 2.f;

			// Tell eyeGUI about position and size of floating frame
			eyegui::setPositionOfFloatingFrame(_pOverlayLayout, _clickVisualizations.at(i).frameIndex, relativePositionX, relativePositionY);
			eyegui::setSizeOfFloatingFrame(_pOverlayLayout, _clickVisualizations.at(i).frameIndex, relativeSize, relativeSize);
		}
	}

	// Erase visualizations which are no more in layout
	_clickVisualizations.erase(
		std::remove_if(
			_clickVisualizations.begin(),
			_clickVisualizations.end(),
            [](const ClickVisualization& i) { return i.fading <= 0; }),
		_clickVisualizations.end());

	// ### TAB INPUT STRUCT ###

	// Create tab input structure (like standard input but in addition with input coordinates in web view space)
	int webViewGazeX = rInput.gazeX - webViewPositionAndSize.x;
	int webViewGazeY = rInput.gazeY - webViewPositionAndSize.y;
	float webViewGazeRelativeX = ((float)webViewGazeX) / ((float)webViewPositionAndSize.width);
	float webViewGazeRelativeY = ((float)webViewGazeY) / ((float)webViewPositionAndSize.height);
	TabInput tabInput(
		rInput.gazeX,
		rInput.gazeY,
		rInput.gazeUsed,
		webViewGazeX,
		webViewGazeY,
		webViewGazeRelativeX,
		webViewGazeRelativeY);

	// Update highlight rectangle of webview
	// TODO: alternative: give webview shared pointer to DOM nodes
	std::vector<Rect> rects;
	for (const auto& rDOMNode : _DOMTextLinks)
	{
		for (const auto& rRect : rDOMNode->GetRects())
		{
			rects.push_back(rRect);
		}
	}
	_upWebView->SetHighlightRects(rects);

	// ### UPDATE COLOR OF GUI ###

	UpdateAccentColor(tpf);

	// ### UPDATE PIPELINES OR MODE ###

	// Decide what to update
	if (_pipelineActive)
	{
		// PIPELINE GETS UPDATED. STANDARD GUI NOT VISIBLE

		// Disable input for panel layout and enable it for pipeline abort layout
		eyegui::setInputUsageOfLayout(_pPanelLayout, false);
		eyegui::setInputUsageOfLayout(_pPipelineAbortLayout, true);

		// Update current pipeline (if there is one)
		if (_pipelines.front()->Update(tpf, tabInput))
		{
			// Remove front element from pipelines
			_pipelines.front()->Deactivate();
			_pipelines.pop_front();

			// Activate new pipeline if there is some
			if (!(_pipelines.empty()))
			{
				_pipelines.front()->Activate();
			}
			else
			{
				// Remember that there is no active pipeline and deactivate GUI to abort it
				SetPipelineActivity(false);
			}
		}
	}
	else
	{
		// MODE GETS UPDATED. STANDARD GUI IS VISIBLE

		// Ask for page resolution
		_timeUntilGetPageResolution -= tpf;
		if (_timeUntilGetPageResolution <= 0)
		{
			_timeUntilGetPageResolution = TAB_GET_PAGE_RES_INTERVAL;
			_pCefMediator->GetPageResolution(this);
		}

		// Enable input for panel layout and disable it for pipeline abort layout
		eyegui::setInputUsageOfLayout(_pPanelLayout, true);
		eyegui::setInputUsageOfLayout(_pPipelineAbortLayout, false);

		// Manual scrolling
		bool showScrollUp = false;
		bool showScrollDown = false;
		if (!_autoScrolling)
		{
			// Scroll up
			if (_scrollingOffsetY > 0)
			{
				showScrollUp = true;
			}

			// Scroll down
			if ((_pageHeight - 1) > (_scrollingOffsetY + webViewPositionAndSize.height))
			{
				showScrollDown = true;
			}
		}

		// Set progress of scrolling
		float scrollableHeight = (_pageHeight - 1) - webViewPositionAndSize.height;
		float progressUp = 1.f;
		float progressDown = 1.f;
		if (scrollableHeight > 0)
		{
			progressUp = _scrollingOffsetY / scrollableHeight;
			progressDown = 1.f - progressUp;
		}
		eyegui::setProgress(_pScrollingOverlayLayout, "scroll_up_progress", progressUp);
		eyegui::setProgress(_pScrollingOverlayLayout, "scroll_down_progress", progressDown);

		// Set visibility of scroll elements
		eyegui::setVisibilityOFloatingFrame(_pScrollingOverlayLayout, _scrollUpProgressFrameIndex, showScrollUp, false, true);
		eyegui::setVisibilityOFloatingFrame(_pScrollingOverlayLayout, _scrollDownProgressFrameIndex, showScrollDown, false, true);
		eyegui::setVisibilityOFloatingFrame(_pScrollingOverlayLayout, _scrollUpSensorFrameIndex, showScrollUp, false, true);
		eyegui::setVisibilityOFloatingFrame(_pScrollingOverlayLayout, _scrollDownSensorFrameIndex, showScrollDown, false, true);

		// Check, that gaze is not upon a fixed element
		bool gazeUponFixed = false;
		for (const auto& rElements : _fixedElements)
		{
			for (const auto& rElement : rElements)
			{
				// Simple box test
				if (tabInput.webViewGazeX > rElement.left
					&& tabInput.webViewGazeY > rElement.top
					&& tabInput.webViewGazeY < rElement.bottom
					&& tabInput.webViewGazeX < rElement.right)
				{
					gazeUponFixed = true;
					break;
				}
			}
			if (gazeUponFixed) { break; }
		}

		// Automatic scrolling. Check whether gaze is available inside webview
		if (_autoScrolling && !tabInput.gazeUsed && tabInput.insideWebView && !gazeUponFixed)
		{
			// Only vertical scrolling supported, yet (TODO: parameters)
			float yOffset = -tabInput.webViewGazeRelativeY + 0.5f;
			yOffset *= 2; // [-1..1]
			bool negative = yOffset < 0;
			yOffset = yOffset * yOffset; // [0..1]
										 // yOffset = (glm::max(0.f, yOffset - 0.05f) / 0.95f); // In the center of view no movement
			yOffset = negative ? -yOffset : yOffset; // [-1..1]

													 // Update the auto scrolling value (TODO: not frame rate independend)
			_autoScrollingValue += tpf * (yOffset - _autoScrollingValue);
		}
		else if (_autoScrollingValue != 0)
		{
			// Fade it out since auto scrolling still ongoing
			if (_autoScrollingValue > 0)
			{
				_autoScrollingValue -= tpf;
				_autoScrollingValue = glm::max(0.f, _autoScrollingValue);
			}
			else
			{
				_autoScrollingValue += tpf;
				_autoScrollingValue = glm::min(0.f, _autoScrollingValue);
			}
		}

		// Use value of auto scrolling to scroll
		_pCefMediator->EmulateMouseWheelScrolling(this, 0.0, (double)(20.f * _autoScrollingValue));

		// Update triggers
		for (auto& upDOMTrigger : _DOMTriggers)
		{
			upDOMTrigger->Update(tpf, tabInput);
		}

		// Check for mode change
		if (_nextMode != _mode)
		{
			// Deactivate old mode
			DeactivateMode(_mode);
			_mode = _nextMode;
			ActivateMode(_mode);
		}

		// Take a look at current mode
		switch (_mode)
		{
		case TabMode::READ:
			break;
		case TabMode::INTERACTION:
			break;
		case TabMode::CURSOR:
			break;
		}
	}
}

void Tab::Draw() const
{
	// Draw WebView
	_upWebView->Draw(
		_webViewParameters,
		_pMaster->GetWindowWidth(),
		_pMaster->GetWindowHeight(),
		_scrollingOffsetX,
		_scrollingOffsetY);

	// Decide what to draw
	if (_pipelineActive)
	{
		// Draw stuff from current pipeline (overlay etc.)
		_pipelines.front()->Draw();
	}
	else
	{
		// Draw triggers
		for (const auto& upDOMTrigger : _DOMTriggers)
		{
			upDOMTrigger->Draw();
		}

		// Pipeline is empty, so first take a look at current mode
		switch (_mode)
		{
		case TabMode::READ:
			break;
		case TabMode::INTERACTION:
			break;
		case TabMode::CURSOR:
			break;
		}

		// Draw debug overlay (do not so while pipeline is rendered)
		if (setup::DRAW_DEBUG_OVERLAY)
		{
			DrawDebuggingOverlay();
		}
	}
}

void Tab::Activate()
{
	// Show layouts
	eyegui::setVisibilityOfLayout(_pOverlayLayout, true, true, false);
	eyegui::setVisibilityOfLayout(_pScrollingOverlayLayout, true, true, false);
	eyegui::setVisibilityOfLayout(_pPanelLayout, true, true, false);

	// Setup switches
	if (_autoScrolling) { eyegui::buttonDown(_pPanelLayout, "auto_scrolling", true); }
	if (_zoomLevel != 1.0) { eyegui::buttonDown(_pPanelLayout, "zoom", true); }

	// Screw color interpolation
	_currentColorAccent = _targetColorAccent;
	_colorInterpolation = 1.f;
}

void Tab::Deactivate()
{
	// Hide layouts
	eyegui::setVisibilityOfLayout(_pOverlayLayout, false, true, false);
	eyegui::setVisibilityOfLayout(_pScrollingOverlayLayout, false, true, false);
	eyegui::setVisibilityOfLayout(_pPanelLayout, false, true, false);

	// TODO: THIS SHOULD NOT BE NECESSARY SINCE _pScrollingOverlayLayout IS HIDDEN! WHY?
	eyegui::setVisibilityOFloatingFrame(_pScrollingOverlayLayout, _scrollUpProgressFrameIndex, false, false, true);
	eyegui::setVisibilityOFloatingFrame(_pScrollingOverlayLayout, _scrollDownProgressFrameIndex, false, false, true);
	eyegui::setVisibilityOFloatingFrame(_pScrollingOverlayLayout, _scrollUpSensorFrameIndex, false, false, true);
	eyegui::setVisibilityOFloatingFrame(_pScrollingOverlayLayout, _scrollDownSensorFrameIndex, false, false, true);

	// Abort pipeline, which also hides GUI for manual abortion
	AbortAndClearPipelines();
}

void Tab::OpenURL(std::string URL)
{
	_url = URL;
	_pCefMediator->RefreshTab(this);
	AbortAndClearPipelines();
}

void Tab::AbortAndClearPipelines()
{
	// Clear pipeline
	_pipelines.clear(); // Destructor of pipeline aborts them
	SetPipelineActivity(false);
}

void Tab::GoForward()
{
	_pCefMediator->GoForward(this);
}

void Tab::GoBack()
{
	_pCefMediator->GoBack(this);
}

void Tab::Reload()
{
	_pCefMediator->ReloadTab(this);
}

eyegui::AbsolutePositionAndSize Tab::CalculateWebViewPositionAndSize() const
{
	return eyegui::getAbsolutePositionAndSizeOfElement(_pPanelLayout, "web_view");
}

void Tab::SetPipelineActivity(bool active)
{
	if (active)
	{
		_pipelineActive = true;
		eyegui::setVisibilityOfLayout(_pPipelineAbortLayout, true, true, true);

		// Deactivate all triggers
		for (auto& upDOMTrigger : _DOMTriggers)
		{
			upDOMTrigger->Deactivate();
		}

		// Deactivate scrolling overlay
		eyegui::setVisibilityOfLayout(_pScrollingOverlayLayout, false, false, true);
	}
	else
	{
		_pipelineActive = false;
		eyegui::setVisibilityOfLayout(_pPipelineAbortLayout, false, false, true);

		// Activate all triggers
		for (auto& upDOMTrigger : _DOMTriggers)
		{
			upDOMTrigger->Activate();
		}

		// Activate scrolling overlay
		eyegui::setVisibilityOfLayout(_pScrollingOverlayLayout, true, true, true);
	}
}

void Tab::UpdateAccentColor(float tpf)
{
	// Move accent color accent towards target accent color
	glm::vec4 colorAccent = _currentColorAccent;
	if (_colorInterpolation < 1.f)
	{
		_colorInterpolation += tpf; // interpolate
		_colorInterpolation = glm::min(_colorInterpolation, 1.f); // clamp
		colorAccent = ((1.f - _colorInterpolation) * _currentColorAccent) + (_colorInterpolation * _targetColorAccent); // calculation of current color
		if (_colorInterpolation == 1.f)
		{
			_currentColorAccent = _targetColorAccent;
		}
	}

	// Create background color
	float backgroundMultiplier = 0.5f;
	float panelBackgroundMultiplier = 0.4f;
	glm::vec4 backgroundAccentColor(
		colorAccent.r * backgroundMultiplier,
		colorAccent.g * backgroundMultiplier,
		colorAccent.b * backgroundMultiplier,
		colorAccent.a);
	glm::vec4 panelBackgroundAccentColor(
		colorAccent.r * panelBackgroundMultiplier,
		colorAccent.g * panelBackgroundMultiplier,
		colorAccent.b * panelBackgroundMultiplier,
		colorAccent.a);

	// Create transparent colors
	float alpha = 0.75f;
	float backgroundAlpha = 0.5f;
	glm::vec4 transparentColorAccent = glm::vec4(
		colorAccent.r,
		colorAccent.g,
		colorAccent.b,
		colorAccent.a * alpha);
	glm::vec4 transparentBackgroundColorAccent = glm::vec4(
		backgroundAccentColor.r,
		backgroundAccentColor.g,
		backgroundAccentColor.b,
		backgroundAccentColor.a * backgroundAlpha);

	// Set color of tab_panel style in pipeline abort overlay
	eyegui::setValueOfStyleAttribute(
		_pPipelineAbortLayout,
		"tab_panel",
		"color",
		RGBAToHexString(colorAccent));

	// Set color of tab_panel style in layout
	eyegui::setValueOfStyleAttribute(
		_pPanelLayout,
		"tab_panel",
		"color",
		RGBAToHexString(colorAccent));

	// Set color of tab_overlay style in overlay layout
	eyegui::setValueOfStyleAttribute(
		_pOverlayLayout,
		"tab_overlay",
		"color",
		RGBAToHexString(colorAccent));
	eyegui::setValueOfStyleAttribute(
		_pOverlayLayout,
		"tab_overlay",
		"background-color",
		RGBAToHexString(backgroundAccentColor));

	// Set color of tab_overlay_noback style in overlay layout
	eyegui::setValueOfStyleAttribute(
		_pOverlayLayout,
		"tab_overlay_noback",
		"color",
		RGBAToHexString(colorAccent));
	eyegui::setValueOfStyleAttribute(
		_pOverlayLayout,
		"tab_overlay_noback",
		"color",
		RGBAToHexString(colorAccent));

	// Set color of tab_overlay_panel style in overlay layout
	eyegui::setValueOfStyleAttribute(
		_pOverlayLayout,
		"tab_overlay_panel",
		"color",
		RGBAToHexString(backgroundAccentColor));
	eyegui::setValueOfStyleAttribute(
		_pOverlayLayout,
		"tab_overlay_panel",
		"background-color",
		RGBAToHexString(panelBackgroundAccentColor));

	// Set color of tab_overlay style in scrolling overlay layout
	eyegui::setValueOfStyleAttribute(
		_pScrollingOverlayLayout,
		"tab_overlay",
		"color",
		RGBAToHexString(colorAccent));

	// Set color and background of tab_overlay_scroll_progress style in scrolling overlay layout
	eyegui::setValueOfStyleAttribute(
		_pScrollingOverlayLayout,
		"tab_overlay_scroll_progress",
		"color",
		RGBAToHexString(transparentColorAccent));
	eyegui::setValueOfStyleAttribute(
		_pScrollingOverlayLayout,
		"tab_overlay_scroll_progress",
		"background-color",
		RGBAToHexString(transparentBackgroundColorAccent));
}

void Tab::ActivateMode(TabMode mode)
{
	switch (mode)
	{
	case TabMode::READ:
		ActivateReadMode();
		break;
	case TabMode::INTERACTION:
		ActivateInteractionMode();
		break;
	case TabMode::CURSOR:
		ActivateCursorMode();
		break;
	}
}

void Tab::DeactivateMode(TabMode mode)
{
	switch (mode)
	{
	case TabMode::READ:
		DeactivateReadMode();
		break;
	case TabMode::INTERACTION:
		DeactivateInteractionMode();
		break;
	case TabMode::CURSOR:
		DeactivateCursorMode();
		break;
	}
}

void Tab::ActivateReadMode()
{
	// TODO
}

void Tab::DeactivateReadMode()
{
	// TODO
}

void Tab::ActivateInteractionMode()
{
	// TODO
}

void Tab::DeactivateInteractionMode()
{
	// TODO
}

void Tab::ActivateCursorMode()
{
	// TODO
}

void Tab::DeactivateCursorMode()
{
	// TODO
}

void Tab::DrawDebuggingOverlay() const
{
	// Reserve variables
	glm::mat4 model, matrix;

	// WebView coordinats
	int webViewX = 0;
	int webViewY = 0;
	int webViewWidth = 0;
	int webViewHeight = 0;
	this->CalculateWebViewPositionAndSize(webViewX, webViewY, webViewWidth, webViewHeight);

	// Projection
	glm::mat4 projection = glm::ortho(0, 1, 0, 1);

	// Define render function
	std::function<void(Rect, bool)> renderRect = [&](Rect rect, bool fixed)
	{
		// Fixed or not
		float scrollX = 0;
		float scrollY = 0;
		if (!fixed)
		{
			scrollX = _scrollingOffsetX;
			scrollY = _scrollingOffsetY;
		}

		// Calculate model matrix
		model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(1.f / _pMaster->GetWindowWidth(), 1.f / _pMaster->GetWindowHeight(), 1.f));
		model = glm::translate(model, glm::vec3(webViewX + rect.left - scrollX, webViewHeight - (rect.bottom - scrollY), 1));
		model = glm::scale(model, glm::vec3(rect.width(), rect.height(), 0));

		// Combine matrics
		matrix = projection * model;

		// Fill uniform with matrix (no need for Bind() since bound in called context)
		_upDebugRenderItem->GetShader()->UpdateValue("matrix", matrix);

		// Render rectangle
		_upDebugRenderItem->Draw(GL_LINE_STRIP);
	};

	// Bind debug render item
	_upDebugRenderItem->Bind();

	// ### DOMTRIGGER ###

	// Set rendering up for DOMTrigger
	_upDebugRenderItem->GetShader()->UpdateValue("color", DOM_TRIGGER_DEBUG_COLOR);

	// Go over all DOMTriggers
	for (const auto& rDOMTrigger : _DOMTriggers)
	{
		// Render rects
		for (const auto rRect : rDOMTrigger->GetDOMRects())
		{
			renderRect(rRect, rDOMTrigger->GetDOMFixed());
		}
	}

	// ### DOMTEXTLINKS ###

	// Set rendering up for DOMTextLink
	_upDebugRenderItem->GetShader()->UpdateValue("color", DOM_TEXT_LINKS_DEBUG_COLOR);

	// Go over all DOMTextLinks
	for (const auto& rDOMTextLink : _DOMTextLinks)
	{
		// Render rects
		for (const auto rRect : rDOMTextLink->GetRects())
		{
			renderRect(rRect, rDOMTextLink->GetFixed());
		}
	}

	// DEBUG
	_upDebugRenderItem->GetShader()->UpdateValue("color", glm::vec3(1.f, 0.f, 1.f));
	for (const auto& rDOMTextLink : _DOMTextLinks)
	{
		if(rDOMTextLink->GetRects().size() == 2)
			renderRect(rDOMTextLink->GetRects()[1], rDOMTextLink->GetFixed());
	}


	// ### FIXED ELEMENTS ###

	// Set rendering up for fixed element
	_upDebugRenderItem->GetShader()->UpdateValue("color", FIXED_ELEMENT_DEBUG_COLOR);

	// Go over all fixed elements vectors
	for (const auto& rFixedElements : _fixedElements)
	{
		// Go over fixed elements
		for (const auto& rFixedElement : rFixedElements)
		{
			// Render rect
			renderRect(rFixedElement, true);
		}
	}
}

void Tab::AddClickVisualization(double x, double y)
{
	// Coordinates of web view
	auto webViewCoordinates = CalculateWebViewPositionAndSize();

	// Structure for click visulization
	ClickVisualization clickVisualization;
	clickVisualization.x = x;
	clickVisualization.y = y;
	clickVisualization.fading = CLICK_VISUALIZATION_DURATION;

	// Position of floating frame
	float relativePositionX = (webViewCoordinates.x + x) / (float)_pMaster->GetWindowWidth();
	float relativePositionY = (webViewCoordinates.y + y) / (float)_pMaster->GetWindowHeight();
	relativePositionX -= CLICK_VISUALIZATION_RELATIVE_SIZE / 2.f;
	relativePositionY -= CLICK_VISUALIZATION_RELATIVE_SIZE / 2.f;

	// Add floating frame to overlay
	clickVisualization.frameIndex = eyegui::addFloatingFrameWithBrick(
		_pOverlayLayout,
		"bricks/TabClickVisualization.beyegui",
		relativePositionX,
		relativePositionY,
		CLICK_VISUALIZATION_RELATIVE_SIZE,
		CLICK_VISUALIZATION_RELATIVE_SIZE,
		true,
		false);

	// Add to vector which is updated per frame
	_clickVisualizations.push_back(clickVisualization);
}
