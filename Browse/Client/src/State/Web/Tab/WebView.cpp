//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "WebView.h"
#include "src/Utils/Texture.h"
#include "externals/GLM/glm/gtc/matrix_transform.hpp"

// Shaders
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
    _upRenderItem = std::unique_ptr<RenderItem>(new RenderItem(vertexShaderSource, fragmentShaderSource));
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
    _upRenderItem->Bind();

    // Calculate model matrix
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(1.f / windowWidth, 1.f / windowHeight, 1.f));
    model = glm::translate(model, glm::vec3(_x, _y, 0));
    model = glm::scale(model, glm::vec3(_width, _height, 1));

    // Projection
    glm::mat4 projection = glm::ortho(0, 1, 0, 1);

    // Combine matrics
    glm::mat4 matrix = projection * model;

    // Fill uniforms
    _upRenderItem->GetShader()->UpdateValue("matrix", matrix);
    _upRenderItem->GetShader()->UpdateValue("centerOffset", parameters.centerOffset);
    _upRenderItem->GetShader()->UpdateValue("zoomPosition", parameters.zoomPosition);
    _upRenderItem->GetShader()->UpdateValue("zoom", parameters.zoom);
    _upRenderItem->GetShader()->UpdateValue("dim", parameters.dim);

    // Bind texture with rendered web page
    _spTexture->Bind();

    // Draw quad
   _upRenderItem->Draw();
}

std::weak_ptr<Texture> WebView::GetTexture()
{
    return _spTexture;
}
