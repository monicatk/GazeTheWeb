//============================================================================
// Distributed under the MIT License.
// Author: Raphael Menges (https://github.com/raphaelmenges)
//============================================================================

#ifndef WEB_VIEW_H
#define WEB_VIEW_H

#include "externals/OGL/gl_core_3_3.h"
#include "externals/GLM/glm/glm.hpp"

// In contrast to eyeGUI is the WebView in OpenGL coordinates (origin left bottom of screen)
class WebView
{
public:

    WebView(int x, int y, int width, int height);
    virtual ~WebView();

    void draw(int windowWidth, int windowHeight, glm::vec2 clickOffset, glm::vec2 clickPositionb, float clickZoom) const;
    GLuint getTextureHandle() const;
    void setPosition(int x, int y);
    void setSize(int width, int height);

private:

    GLuint mTextureHandle;
    int mX;
    int mY;
    int mWidth;
    int mHeight;
    GLuint mShaderProgram;
    GLuint mVertexArrayObject;
};

#endif // WEB_VIEW_H
