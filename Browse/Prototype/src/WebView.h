//============================================================================
// Distributed under the MIT License.
// Author: Raphael Menges (https://github.com/raphaelmenges)
//============================================================================

#ifndef WEB_VIEW_H
#define WEB_VIEW_H

#include "externals/OGL/gl_core_3_3.h"
#include "externals/GLM/glm/glm.hpp"
#include "externals/eyeGUI/include/eyeGUI.h"

#include <vector>

// Constants
const float INPUT_FIELD_BUTTON_SIZE = 0.15f;

// Forward declaration
class WebView;

// Button listener
class InputFieldButtonListener : public eyegui::ButtonListener
{
public:

    // Constructor
    InputFieldButtonListener(WebView* pWebView);

    // Callbacks
    void virtual hit(eyegui::Layout* pLayout, std::string id);
    void virtual down(eyegui::Layout* pLayout, std::string id);
    void virtual up(eyegui::Layout* pLayout, std::string id);

private:

    // Members
    WebView* mpWebView;
};

// In contrast to eyeGUI is the WebView in OpenGL coordinates (origin left bottom of screen)
class WebView
{
public:

    WebView(eyegui::GUI* pGUI, void(*inputFieldCallback) (unsigned int index), int x, int y, int width, int height);
    virtual ~WebView();
    void update(int windowWidth, int windowHeight, double scrollOffsetY, double scrollingInLastFrame);
    void draw(glm::vec2 clickOffset, glm::vec2 clickPosition, float clickZoom) const;
    GLuint getTextureHandle() const;
    void setPosition(int x, int y);
    void setSize(int width, int height);
    void setInputFields(std::vector<glm::vec2> positions);
    void inputFieldButtonCallback(unsigned int index);
    void setVisibilityOfOverlay(bool visible);

private:

    GLuint mTextureHandle;
    int mX;
    int mY;
    int mWidth;
    int mHeight;
    int mWindowWidth;
    int mWindowHeight;
    GLuint mShaderProgram;
    GLuint mVertexArrayObject;
    eyegui::GUI* mpGUI;
    eyegui::Layout* mpLayout;
    std::vector<unsigned int> mInputFieldIndices;
    std::shared_ptr<InputFieldButtonListener> mspInputFieldButtonListener;
    void(*mInputFieldCallback) (unsigned int index);
    std::vector<glm::vec2> mInputFieldPositions;
    double mOldScrollOffsetY;
    int mSmoothScrollOffsetY;
};

#endif // WEB_VIEW_H
