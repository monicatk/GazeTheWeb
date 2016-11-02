//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// View on web which has Texture and renders it onto OpenGL quad.
// All input is assumed to have origin in upper left corner.

#ifndef WEBVIEW_H_
#define WEBVIEW_H_

#include "src/State/Web/Tab/WebViewParameters.h"
#include "src/Utils/RenderItem.h"
#include "src/CEF/Data/Rect.h"
#include "src/Utils/glmWrapper.h"
#include "src/Utils/Framebuffer.h"
#include "externals/OGL/gl_core_3_3.h"
#include <memory>
#include <vector>

// Forward declarations
class Texture;

// Class
class WebView
{
public:

    // Constructor
    WebView(
		int x,
		int y,
		int width,
		int height);

    // Destructor
    virtual ~WebView();

    // Update values. Coordinates in pixels, origin is upper left corner
    void Update(
        int x,
        int y,
        int width,
        int height);

    // Draw
    void Draw(
		const WebViewParameters& parameters,
		int windowWidth,
		int windowHeight,
		double scrollingOffsetX,
		double scrollingOffsetY) const;

    // Getter for weak pointer of texture
    std::weak_ptr<Texture> GetTexture();

    // Set rects which are not dimmed
    void SetHighlightRects(std::vector<Rect> rects);

	// Getter for values of GUI element. Resolution may not be same as web page rendering
	int GetX() const;
	int GetY() const;
	int GetWidth() const;
	int GetHeight() const;

	// Getter for web view resolution
	int GetResolutionX() const;
	int GetResolutionY() const;

private:

    // Texture object which belongs here but filled by CEF and read maybe by other
    std::shared_ptr<Texture> _spTexture;

    // Render item
    std::unique_ptr<RenderItem> _upSimpleRenderItem;
    std::unique_ptr<RenderItem> _upCompositeRenderItem;

    // Current values
    int _x = 0;
    int _y = 0;
    int _width = 0;
    int _height = 0;

    // Vector with rects used for highlighting
    std::vector<Rect> _rects;

    // Framebuffer to render highlights etc on webpage and later zoom in
    std::unique_ptr<Framebuffer> _upFramebuffer;
};

#endif // WEBVIEW_H_
