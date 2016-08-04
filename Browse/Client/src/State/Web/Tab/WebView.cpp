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
"uniform vec4 textureCoordinate;\n" // minU, minV, maxU, maxV. OpenGL coordinate system!
"void main() {\n"
"    gl_Position = vec4(position.zw, 0.0, 1.0);\n" // upper right corner
"    uv = vec2(textureCoordinate.zw);\n"
"    EmitVertex();\n"
"    gl_Position = vec4(position.xw, 0.0, 1.0);\n" // upper left corner
"    uv = vec2(textureCoordinate.xw);\n"
"    EmitVertex();\n"
"    gl_Position = vec4(position.zy, 0.0, 1.0);\n" // lower right corner
"    uv = vec2(textureCoordinate.zy);\n"
"    EmitVertex();\n"
"    gl_Position = vec4(position.xy, 0.0, 1.0);\n" // lower left corner
"    uv = vec2(textureCoordinate.xy);\n"
"    EmitVertex();\n"
"    EndPrimitive();\n"
"}\n";

const std::string simpleFragmentShaderSource =
"#version 330 core\n"
"in vec2 uv;\n"
"out vec4 fragColor;\n"
"uniform sampler2D tex;\n"
"uniform float dim;\n"
"void main() {\n"
"	vec4 color = texture(tex, uv);\n"
"   fragColor = vec4(color.rgb * (1.0 - dim), color.a);\n"
"}\n";

const std::string compositionFragmentShaderSource =
"#version 330 core\n"
"in vec2 uv;\n"
"out vec4 fragColor;\n"
"uniform sampler2D tex;\n"
"uniform vec2 centerOffset;\n"
"uniform vec2 zoomPosition;\n"
"uniform float zoom;\n"
"void main() {\n"
"   vec2 coords = uv;\n"
"   coords += centerOffset;" // Move towards center
"   coords -= zoomPosition;" // Move zoom position to origin
"   coords *= zoom;" // Scale coords
"   coords += zoomPosition;" // Move it back
"   fragColor = texture(tex, coords);\n"
"}\n";

WebView::WebView(int renderWidth, int renderHeight)
{
    // Generate texture
    _spTexture = std::shared_ptr<Texture>(new Texture(renderWidth, renderHeight, GL_RGBA, Texture::Filter::LINEAR, Texture::Wrap::BORDER));

    // Render items
    _upSimpleRenderItem = std::unique_ptr<RenderItem>(new RenderItem(vertexShaderSource, geometryShaderSource, simpleFragmentShaderSource));
    _upCompositeRenderItem = std::unique_ptr<RenderItem>(new RenderItem(vertexShaderSource, geometryShaderSource, compositionFragmentShaderSource));

    // Framebuffer
    _upFramebuffer = std::unique_ptr<Framebuffer>(new Framebuffer(renderWidth, renderHeight));
    _upFramebuffer->Bind();
    _upFramebuffer->AddAttachment(Framebuffer::ColorFormat::RGB);
    _upFramebuffer->Unbind();

    // _x, _y, _width and _height are set at first update
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
    // Update framebuffer size
    if((width != _width) || (height != _height))
    {
        _upFramebuffer->Bind();
        _upFramebuffer->Resize(width, height);
        _upFramebuffer->Unbind();
    }

    // Set members
    _x = x;
    _y = y;
    _width = width;
    _height = height;
}

void WebView::Draw(const WebViewParameters& parameters, int windowWidth, int windowHeight) const
{
    // ### FILL FRAMEBUFFER ###

    // Just render to framebuffer
    _upFramebuffer->Bind();

    // Rescue current viewport and set own which fits rendered webpage
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glViewport(0, 0, _width, _height);

    // Bind render item
    _upSimpleRenderItem->Bind();

    // Bind texture with rendered web page
    _spTexture->Bind();

    // Fill uniforms
    _upSimpleRenderItem->GetShader()->UpdateValue("position", glm::vec4(-1.f, -1.f, 1.f, 1.f)); // normalized device coordinates
    _upSimpleRenderItem->GetShader()->UpdateValue("textureCoordinate", glm::vec4(0.f, 1.f, 1.f, 0.f)); // using texture coordinates to flip image in v direction
    _upSimpleRenderItem->GetShader()->UpdateValue("dim", parameters.dim);

    // Draw webpage completely into framebuffer
    _upSimpleRenderItem->Draw(GL_POINTS);

    // Render highlighting
    if(parameters.highlight > 0)
    {
        // TODO: use value from highlight or so
        // For now: just reset dimming to zero for the rect rendering
        _upSimpleRenderItem->GetShader()->UpdateValue("dim", 0.f);

        // Go over rects and render them
        for(const Rect& rRect : _rects)
        {
            // TODO: implement scrolling

            // Setup position
            _upSimpleRenderItem->GetShader()->UpdateValue(
                "position",
                glm::vec4(
                    (((float)rRect.left / (float)_width) * 2.f) - 1.f,
                    ((((float)(_height - rRect.bottom)) / (float)_height) * 2.f) - 1.f,
                    (((float)rRect.right / (float)_width) * 2.f) - 1.f,
                    ((((float)(_height - rRect.top)) / (float)_height) * 2.f) - 1.f)); // normalized device coordinates

            // Setup texture coordinate
            _upSimpleRenderItem->GetShader()->UpdateValue(
                "textureCoordinate",
                glm::vec4(
                    (float)rRect.left / (float)_width,
                    1.f - (float)(_height - rRect.bottom) / (float)_height,
                    (float)rRect.right / (float)_width,
                    1.f - (float)(_height - rRect.top) / (float)_height)); // using texture coordinates to flip image in v direction

            // Draw the quad
            _upSimpleRenderItem->Draw(GL_POINTS);
        }
    }

    // Restore old viewport
    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

    // Unbind framebuffer
    _upFramebuffer->Unbind();

    // ### COMPOSITING INCLUSIVE ZOOMING ###

    // Render composited inclusive zooming
    _upCompositeRenderItem->Bind();

    // Bind texture with webpage and custom rendered elements
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _upFramebuffer->GetAttachment(0));

    // Fill uniforms (TODO: here, coordinate sytem is not completely correctly translated. Would be only a problem at vertical transformation
    _upCompositeRenderItem->GetShader()->UpdateValue(
        "position",
        glm::vec4(
            ((_x / (float)windowWidth) * 2.f) - 1.f, // minX
            ((_y / (float)windowHeight) * 2.f) - 1.f, // minY
            (((_x + _width) / (float)windowWidth) * 2.f) - 1.f, // maxX
            (((_y + _height) / (float)windowHeight) * 2.f) - 1.f // maxY
            )); // normalized device coordinates
    _upCompositeRenderItem->GetShader()->UpdateValue("textureCoordinate", glm::vec4(0.f, 0.f, 1.f, 1.f)); // everything is rendered correctly into framebuffer, just display it
    _upCompositeRenderItem->GetShader()->UpdateValue("centerOffset", glm::vec2(parameters.centerOffset.x, parameters.centerOffset.y)); // change origin from upper left to lower left
    _upCompositeRenderItem->GetShader()->UpdateValue("zoomPosition", glm::vec2(parameters.zoomPosition.x, parameters.zoomPosition.y)); // change origin from upper left to lower left
    _upCompositeRenderItem->GetShader()->UpdateValue("zoom", parameters.zoom);
    _upCompositeRenderItem->Draw(GL_POINTS);
}

std::weak_ptr<Texture> WebView::GetTexture()
{
    return _spTexture;
}

void WebView::SetHighlightRects(std::vector<Rect> rects)
{
    _rects = rects;
}
