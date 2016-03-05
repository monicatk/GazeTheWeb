//============================================================================
// Distributed under the MIT License.
// Author: Raphael Menges (https://github.com/raphaelmenges)
//============================================================================

#include "WebView.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

// Geometry
const std::vector<float> quadVertices =
{
    0,0,0, 1,0,0, 1,1,0,
    1,1,0, 0,1,0, 0,0,0
};

const std::vector<float> quadTextureCoordinates =
{
    0,0, 1,0, 1,1,
    1,1, 0,1, 0,0
};

// Shaders
static const char *vertexShaderSource =
"#version 330 core\n"
"in vec3 posAttr;\n"
"in vec2 uvAttr;\n"
"out vec2 uv;\n"
"uniform mat4 matrix;\n"
"void main() {\n"
"   uv = uvAttr;\n"
"   gl_Position = matrix * vec4(posAttr, 1);\n"
"}\n";

static const char *fragmentShaderSource =
"#version 330 core\n"
"in vec2 uv;\n"
"out vec4 fragColor;\n"
"uniform sampler2D tex;\n"
"uniform vec2 clickOffset;\n"
"uniform vec2 clickPosition;\n"
"uniform float clickZoom;\n"
"void main() {\n"
"   vec2 coords = vec2(uv.s, 1.0 - uv.t);\n"
"   coords += clickOffset;" // Move towards center of web view
"   coords -= clickPosition;" // Move click position to origin
"   coords *= clickZoom;" // Scale coords
"   coords += clickPosition;" // Move it back
"   fragColor = texture(tex, coords);\n"
"}\n";

// Implementation
WebView::WebView(int x, int y, int width, int height)
{
    // Initialize members
    setPosition(x,y);
    setSize(width, height);

    // Vertex shader
    int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Fragment shader
    int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Create program
    mShaderProgram = glCreateProgram();
    glAttachShader(mShaderProgram, vertexShader);
    glAttachShader(mShaderProgram, fragmentShader);
    glLinkProgram(mShaderProgram);

    // Delete shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    //Vertex buffer object on GPU
    unsigned int vertexBuffer = 0; // index of VBO
    glGenBuffers(1, &vertexBuffer); // generate VBO
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer); // set as current VBO
    glBufferData(GL_ARRAY_BUFFER, quadVertices.size() * sizeof(float), quadVertices.data(), GL_STATIC_DRAW); // copy data

    // UV buffer object on GPU
    unsigned int textureCoordinateBuffer = 0; // index of VBO
    glGenBuffers(1, &textureCoordinateBuffer); // generate VBO
    glBindBuffer(GL_ARRAY_BUFFER, textureCoordinateBuffer); // set as current VBO
    glBufferData(GL_ARRAY_BUFFER, quadTextureCoordinates.size() * sizeof(float), quadTextureCoordinates.data(), GL_STATIC_DRAW); // copy data

    // Vertex Array Object
    glGenVertexArrays(1, &mVertexArrayObject);
    glBindVertexArray(mVertexArrayObject);

    // Vertices
    int posAttr = glGetAttribLocation(mShaderProgram, "posAttr");
    glEnableVertexAttribArray(posAttr);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glVertexAttribPointer(posAttr, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    // Texture coordinates
    int uvAttrib = glGetAttribLocation(mShaderProgram, "uvAttr");
    glEnableVertexAttribArray(uvAttrib);
    glBindBuffer(GL_ARRAY_BUFFER, textureCoordinateBuffer);
    glVertexAttribPointer(uvAttrib, 2, GL_FLOAT, GL_FALSE, 0, NULL);

    // Set up texture before CEF initialization
    mTextureHandle = 0;
    glGenTextures(1, &mTextureHandle);
    glBindTexture(GL_TEXTURE_2D, mTextureHandle);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

WebView::~WebView()
{
    // TODO: Delete OpenGL stuff (Just a prototype :-P )
}

void WebView::draw(int windowWidth, int windowHeight, glm::vec2 clickOffset, glm::vec2 clickPosition, float clickZoom) const
{
    glUseProgram(mShaderProgram);

    // Transformation
    glm::mat4 projection = glm::ortho(0, 1, 0, 1);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(1.f / windowWidth, 1.f / windowHeight, 1.f));
    model = glm::translate(model, glm::vec3(mX, mY, 0));
    model = glm::scale(model, glm::vec3(mWidth, mHeight, 1));

    glm::mat4 matrix = projection * model;

    GLuint uniformPosition = glGetUniformLocation(mShaderProgram, "matrix");
    glUniformMatrix4fv(uniformPosition, 1, GL_FALSE, glm::value_ptr(matrix));

    // Clicking
    glUniform2fv(glGetUniformLocation(mShaderProgram, "clickOffset"), 1, glm::value_ptr(clickOffset));
    glUniform2fv(glGetUniformLocation(mShaderProgram, "clickPosition"), 1, glm::value_ptr(clickPosition));
    glUniform1f(glGetUniformLocation(mShaderProgram, "clickZoom"), clickZoom);

    // Draw view
    glBindVertexArray(mVertexArrayObject);
    glBindTexture(GL_TEXTURE_2D, mTextureHandle);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

GLuint WebView::getTextureHandle() const
{
    return mTextureHandle;
}

void WebView::setPosition(int x, int y)
{
    mX = x;
    mY = y;
}

void WebView::setSize(int width, int height)
{
    mWidth = width;
    mHeight = height;
}
