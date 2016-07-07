//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// View on web which has Texture and renders it onto OpenGL quad.

#ifndef WEBVIEW_H_
#define WEBVIEW_H_

#include "src/State/Web/Tab/WebViewParameters.h"
#include "src/Utils/RenderItem.h"
#include "externals/OGL/gl_core_3_3.h"
#include "submodules/glm/glm/glm.hpp"
#include <memory>

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

    // Update values. Coordinates in pixels
    void Update(
        int x,
        int y,
        int width,
        int height);

    // Draw
    void Draw(const WebViewParameters& parameters, int windowWidth, int windowHeight) const;

    // Getter for weak pointer of texture
    std::weak_ptr<Texture> GetTexture();

private:

    // Texture object which belongs here but filled by CEF and read maybe by other
    std::shared_ptr<Texture> _spTexture;

    // Render item
    std::unique_ptr<RenderItem> _upRenderItem;

    // Current values
    int _x = 0;
    int _y = 0;
    int _width = 0;
    int _height = 0;
};

#endif // WEBVIEW_H_
