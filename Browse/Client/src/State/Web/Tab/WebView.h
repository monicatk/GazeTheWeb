//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// View on web which has Texture and renders it onto OpenGL quad.

#ifndef WEBVIEW_H_
#define WEBVIEW_H_

#include "src/State/Web/Tab/WebViewParameters.h"
#include "src/Utils/RenderItem.h"
#include "src/Utils/Rect.h"
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
    WebView(int renderWidth, int renderHeight);

    // Destructor
    virtual ~WebView();

    // Update values. Coordinates in pixels, origin is upper left corner
    void Update(
        int x,
        int y,
        int width,
        int height);

    // Draw
    void Draw(const WebViewParameters& parameters, int windowWidth, int windowHeight) const;

    // Getter for weak pointer of texture
    std::weak_ptr<Texture> GetTexture();

    // Set rects which can be highlighted
    void SetHighlightRects(std::vector<Rect> rects);

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
