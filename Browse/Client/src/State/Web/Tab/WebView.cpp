//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "WebView.h"
#include "src/Utils/Texture.h"
#include "submodules/glm/glm/gtc/matrix_transform.hpp"

// Shaders
const std::string vertexShaderSource =
"#version 330 core\n"
"void main() {\n"
"}\n";

const std::string geometryShaderSource =
"#version 330 core\n"
"layout(points) in;\n"
"layout(triangle_strip, max_vertices = 4) out;\n"
"out vec2 uv;\n"
"uniform vec4 position;\n" // minX, minY, maxX, maxY. OpenGL coordinate system!
"void main() {\n"
"    gl_Position = vec4(position.zw, 0.0, 1.0);\n"
"    uv = vec2(1.0, 1.0);\n"
"    EmitVertex();\n"
"    gl_Position = vec4(position.xw, 0.0, 1.0);\n"
"    uv = vec2(0.0, 1.0);\n"
"    EmitVertex();\n"
"    gl_Position = vec4(position.zy, 0.0, 1.0);\n"
"    uv = vec2(1.0, 0.0);\n"
"    EmitVertex();\n"
"    gl_Position = vec4(position.xy, 0.0, 1.0);\n"
"    uv = vec2(0.0, 0.0);\n"
"    EmitVertex();\n"
"    EndPrimitive();\n"
"}\n";

const std::string fragmentShaderSource =
"#version 330 core\n"
"in vec2 uv;\n"
"out vec4 fragColor;\n"
"uniform sampler2D tex;\n"
"uniform vec2 centerOffset;\n"
"uniform vec2 zoomPosition;\n"
"uniform float zoom;\n"
"uniform float dim;\n"
"void main() {\n"
"   vec2 coords = vec2(uv.s, 1.0 - uv.t);\n"
"   coords += centerOffset;" // Move towards center
"   coords -= zoomPosition;" // Move zoom position to origin
"   coords *= zoom;" // Scale coords
"   coords += zoomPosition;" // Move it back
"	vec4 color = texture(tex, coords);\n"
"   fragColor = vec4(color.rgb * (1.0 - dim), color.a);\n"
"}\n";

WebView::WebView(int renderWidth, int renderHeight)
{
    // Generate texture
    _spTexture = std::shared_ptr<Texture>(new Texture(renderWidth, renderHeight, GL_RGBA, Texture::Filter::LINEAR, Texture::Wrap::BORDER));

    // Render item
    _upRenderItem = std::unique_ptr<RenderItem>(new RenderItem(vertexShaderSource, geometryShaderSource, fragmentShaderSource));
}

WebView::~WebView()
{
    // Nothing to do
}

void WebView::Update(
    int x,
    int y,
    int width,
    int height)
{
    _x = x;
    _y = y;
    _width = width;
    _height = height;
}

void WebView::Draw(const WebViewParameters& parameters, int windowWidth, int windowHeight) const
{
    // Bind render item
    _upRenderItem->Bind();

    // Bind texture with rendered web page
    _spTexture->Bind();

    // Create function to compute position vector for shader (OpenGL coordinates)
    std::function<glm::vec4(float, float, float, float)> posCalc = [&](float x, float y, float width, float height)
    {
        glm::vec4 position;
        position.x = ((x / (float)windowWidth) * 2.f) - 1.f; // minX
        position.y = ((y / (float)windowHeight) * 2.f) - 1.f; // minY
        position.z = (((x + width) / (float)windowWidth) * 2.f) - 1.f; // maxX
        position.w = (((y + height) / (float)windowHeight) * 2.f) - 1.f;; // maxY
        return position;
    };

    // Fill uniforms
    _upRenderItem->GetShader()->UpdateValue("position", posCalc(_x, _y, _width, _height)); // normalized device coordinates
    _upRenderItem->GetShader()->UpdateValue("centerOffset", parameters.centerOffset);
    _upRenderItem->GetShader()->UpdateValue("zoomPosition", parameters.zoomPosition);
    _upRenderItem->GetShader()->UpdateValue("zoom", parameters.zoom);
    _upRenderItem->GetShader()->UpdateValue("dim", parameters.dim);

    // Draw quad
    _upRenderItem->Draw(GL_POINTS);

    // Render highlighting
    if(parameters.highlight > 0)
    {
        // TODO: use value from highlight or so
        // For now: just reset dimming to zero for the rect rendering
        _upRenderItem->GetShader()->UpdateValue("dim", 0.f);

        // Go over rects and render them
        for(const Rect& rRect : _rects)
        {
            // Setup position
            _upRenderItem->GetShader()->UpdateValue(
                "position",
                posCalc(
                    rRect.left,
                    rRect.bottom,
                    rRect.width(),
                    rRect.height())); // normalized device coordinates

            // Draw the quad
            _upRenderItem->Draw(GL_POINTS);
        }
    }
}

std::weak_ptr<Texture> WebView::GetTexture()
{
    return _spTexture;
}

void WebView::SetHighlightRects(std::vector<Rect> rects)
{
    _rects = rects;
}
