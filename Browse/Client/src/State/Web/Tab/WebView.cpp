//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "WebView.h"
#include "src/Utils/Texture.h"
#include "externals/GLM/glm/gtc/matrix_transform.hpp"
#include "externals/GLM/glm/gtc/type_ptr.hpp"

// Geometry
const float quadVertices[] =
{
    0,0,0, 1,0,0, 1,1,0,
    1,1,0, 0,1,0, 0,0,0
};

const float quadTextureCoordinates[] =
{
    0,0, 1,0, 1,1,
    1,1, 0,1, 0,0
};

// Shaders
const char *vertexShaderSource =
"#version 330 core\n"
"in vec3 posAttr;\n"
"in vec2 uvAttr;\n"
"out vec2 uv;\n"
"uniform mat4 matrix;\n"
"void main() {\n"
"   uv = uvAttr;\n"
"   gl_Position = matrix * vec4(posAttr, 1);\n"
"}\n";

const char *fragmentShaderSource =
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

    // Vertex shader
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Fragment shader
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Create program
    _shaderProgram = glCreateProgram();
    glAttachShader(_shaderProgram, vertexShader);
    glAttachShader(_shaderProgram, fragmentShader);
    glLinkProgram(_shaderProgram);

    // Delete shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    //Vertex buffer object on GPU
    unsigned int vertexBuffer = 0; // index of VBO
    glGenBuffers(1, &vertexBuffer); // generate VBO
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer); // set as current VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW); // copy data
                                                                                     // UV buffer object on GPU
    unsigned int textureCoordinateBuffer = 0; // index of VBO
    glGenBuffers(1, &textureCoordinateBuffer); // generate VBO
    glBindBuffer(GL_ARRAY_BUFFER, textureCoordinateBuffer); // set as current VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadTextureCoordinates), quadTextureCoordinates, GL_STATIC_DRAW); // copy data

    // Vertex Array Object
    glGenVertexArrays(1, &_vertexArrayObject);
    glBindVertexArray(_vertexArrayObject);

    // Vertices
    int posAttr = glGetAttribLocation(_shaderProgram, "posAttr");
    glEnableVertexAttribArray(posAttr);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glVertexAttribPointer(posAttr, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    // Texture coordinates
    int uvAttrib = glGetAttribLocation(_shaderProgram, "uvAttr");
    glEnableVertexAttribArray(uvAttrib);
    glBindBuffer(GL_ARRAY_BUFFER, textureCoordinateBuffer);
    glVertexAttribPointer(uvAttrib, 2, GL_FLOAT, GL_FALSE, 0, NULL);
}

WebView::~WebView()
{
    // Delete OpenGL objects
    glDeleteProgram(_shaderProgram);
    glDeleteVertexArrays(1, &_vertexArrayObject);
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
    glUseProgram(_shaderProgram);

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
    GLuint uniformPosition = glGetUniformLocation(_shaderProgram, "matrix");
    glUniformMatrix4fv(uniformPosition, 1, GL_FALSE, glm::value_ptr(matrix));
    glUniform2fv(glGetUniformLocation(_shaderProgram, "centerOffset"), 1, glm::value_ptr(parameters.centerOffset));
    glUniform2fv(glGetUniformLocation(_shaderProgram, "zoomPosition"), 1, glm::value_ptr(parameters.zoomPosition));
    glUniform1f(glGetUniformLocation(_shaderProgram, "zoom"), parameters.zoom);
    glUniform1f(glGetUniformLocation(_shaderProgram, "dim"), parameters.dim);

    // Bind texture with rendered web page
    _spTexture->Bind();

    // Draw viewy
    glBindVertexArray(_vertexArrayObject);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

std::weak_ptr<Texture> WebView::GetTexture()
{
    return _spTexture;
}
