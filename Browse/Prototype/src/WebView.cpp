//============================================================================
// Distributed under the MIT License.
// Author: Raphael Menges (https://github.com/raphaelmenges)
//============================================================================

#include "WebView.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <iostream>

// ### INPUT FIELD BUTTON LISTENER ###

InputFieldButtonListener::InputFieldButtonListener(WebView* pWebView)
{
    mpWebView = pWebView;
}

void InputFieldButtonListener::hit(eyegui::Layout* pLayout, std::string id)
{
    // Nothing to do
}

void InputFieldButtonListener::down(eyegui::Layout* pLayout, std::string id)
{
    // Get index and tell web view (just id to unsigned int)
    mpWebView->inputFieldButtonCallback((unsigned int)(std::atoi(id.c_str())));
}

void InputFieldButtonListener::up(eyegui::Layout* pLayout, std::string id)
{
    // Nothing to do
}

// ### WEB VIEW ###

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
WebView::WebView(eyegui::GUI* pGUI, void(*inputFieldCallback) (unsigned int index), int x, int y, int width, int height)
{
    // Initialize members
    mpGUI = pGUI;
    mInputFieldCallback = inputFieldCallback;
    setPosition(x,y);
    setSize(width, height);
    mOldScrollOffsetY = 0;
    mSmoothScrollOffsetY = 0;

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

    // Create layout for overlays
    mpLayout = eyegui::addLayout(mpGUI, "layouts/Empty.xeyegui", true);
    eyegui::moveLayoutToBack(mpGUI, mpLayout);

    // Create button listener
    mspInputFieldButtonListener = std::shared_ptr<InputFieldButtonListener>(new InputFieldButtonListener(this));
}

WebView::~WebView()
{
    // TODO: Delete OpenGL stuff (Just a prototype :-P )
}

void WebView::update(int windowWidth, int windowHeight, double scrollOffsetY, double scrollingInLastFrame)
{
    // Save window size to members
    mWindowWidth = windowWidth;
    mWindowHeight = windowHeight;

    // Smooth scrolling
    mSmoothScrollOffsetY += scrollingInLastFrame; // Add scrolling of frame
    mSmoothScrollOffsetY -= scrollOffsetY - mOldScrollOffsetY; // Subtract done scrolling

    // Only to expensive stuff when necessary
    if(mOldScrollOffsetY != scrollOffsetY)
    {
        mOldScrollOffsetY = scrollOffsetY;

        // Update position of input field buttons
        for(int i = 0; i < (int)mInputFieldIndices.size(); i++)
        {
            // Prepare values
            unsigned int floatingFrameIndex = mInputFieldIndices.at(i);
            glm::vec2 position = mInputFieldPositions.at(i);
            float x = (((float)mX + position.x) / (float)mWindowWidth) - (INPUT_FIELD_BUTTON_SIZE / 2.f);
            float y = (((float)((mWindowHeight - mHeight) + (position.y - (int)scrollOffsetY)) / (float)mWindowHeight) - (INPUT_FIELD_BUTTON_SIZE / 2.f)); // TODO: i think that is not correct, mY missing in formula

            // Tell floating frame about it
            eyegui::setPositionOfFloatingFrame(mpLayout, floatingFrameIndex, x, y);
        }
    }
}

void WebView::draw(glm::vec2 clickOffset, glm::vec2 clickPosition, float clickZoom) const
{
    glUseProgram(mShaderProgram);

    // Transformation
    glm::mat4 projection = glm::ortho(0, 1, 0, 1);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(1.f / mWindowWidth, 1.f / mWindowHeight, 1.f));
    // model = glm::translate(model, glm::vec3(mX, mY - mSmoothScrollOffsetY, 0)); // TODO: testing smooth scrolling.
    model = glm::translate(model, glm::vec3(mX, mY, 0));
    model = glm::scale(model, glm::vec3(mWidth, mHeight, 1));

    glm::mat4 matrix = projection * model;

    GLuint uniformPosition = glGetUniformLocation(mShaderProgram, "matrix");
    glUniformMatrix4fv(uniformPosition, 1, GL_FALSE, glm::value_ptr(matrix));

    // Clicking
    glUniform2fv(glGetUniformLocation(mShaderProgram, "clickOffset"), 1, glm::value_ptr(clickOffset));
    glUniform2fv(glGetUniformLocation(mShaderProgram, "clickPosition"), 1, glm::value_ptr(clickPosition));
    glUniform1f(glGetUniformLocation(mShaderProgram, "clickZoom"), clickZoom);

    // Draw viewy
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

void WebView::setInputFields(std::vector<glm::vec2> positions)
{
    // Remember positions for updates
    mInputFieldPositions = positions;

    // Delete old floating frames
    for (unsigned int index : mInputFieldIndices)
    {
        eyegui::removeFloatingFrame(mpLayout, index, true);
    }

    // Forget about old floating frames
    mInputFieldIndices.clear();

    // Add new floating frames
    unsigned int index = 0;
    for (glm::vec2& position : mInputFieldPositions)
    {
        // Create id map for that button
        std::map<std::string, std::string> idMap;
        std::string id = std::to_string(index);
        idMap["button"] = id; // create id of that button, just use index

        // Layout is in fullscreen and web view has different coordinate system than eyeGUI
        float x = (((float)mX + position.x) / (float)mWindowWidth) - (INPUT_FIELD_BUTTON_SIZE / 2.f);
        float y = (((float)((mWindowHeight - mHeight) + position.y) / (float)mWindowHeight) - (INPUT_FIELD_BUTTON_SIZE / 2.f)); // TODO: i think that is not correct, mY missing in formula
        mInputFieldIndices.push_back(
            eyegui::addFloatingFrameWithBrick(
                mpLayout,
                "bricks/InputButton.beyegui",
                x,
                y,
                INPUT_FIELD_BUTTON_SIZE,
                INPUT_FIELD_BUTTON_SIZE,
                idMap,
                true,
                true));

        // Add listener for that button
        eyegui::registerButtonListener(mpLayout, id, mspInputFieldButtonListener);

        // Increment index of input field
        index++;
    }
}

void WebView::inputFieldButtonCallback(unsigned int index)
{
    mInputFieldCallback(index);
}

void WebView::setVisibilityOfOverlay(bool visible)
{
    eyegui::setVisibilityOfLayout(mpLayout, visible, false, true);
}
