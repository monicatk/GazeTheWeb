//============================================================================
// Distributed under the MIT License.
// Author: Raphael Menges (https://github.com/raphaelmenges)
//============================================================================

#ifndef SIMPLE_RENDERER_H_
#define SIMPLE_RENDERER_H_

#include "include/cef_client.h"
#include "include/cef_render_handler.h"

#include "include/base/cef_logging.h"

#include "externals/OGL/gl_core_3_3.h"

// Does offscreen rendering
class SimpleRenderer : public CefRenderHandler
{
public:

    SimpleRenderer(unsigned int textureHandle, int* pWidth, int* pHeight)
    {
        mTextureHandle = textureHandle;
        mpWidth = pWidth;
        mpHeight = pHeight;
        mFirstPaint = true;
        mRecreateTexture = false;
    } // give it pointer to texture, width and height

    // Tell CEF3 size of texture to render to
    bool GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect)
    {
        rect = CefRect(0, 0, *mpWidth, *mpHeight);
        mRecreateTexture = true;
        return true;
    }

    // When paint happens, copy pixels over RAM to texture (directly GPU mapping would be awesome)
    void OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList &dirtyRects, const void *buffer, int width, int height)
    {
        glBindTexture(GL_TEXTURE_2D, mTextureHandle);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // TODO: should be unset!!!

        if(mFirstPaint)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, buffer);
            mFirstPaint = false;
        }
        else if(mRecreateTexture)
        {
            // glDeleteTextures(1, &mTextureHandle); // why commented?
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, buffer);
            mRecreateTexture = false;
        }
        else
        {
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_BGRA, GL_UNSIGNED_BYTE, buffer);
        }
    }

private:

    // CefBase interface
    IMPLEMENT_REFCOUNTING(SimpleRenderer)

    unsigned int mTextureHandle;
    bool mFirstPaint;
    bool mRecreateTexture;
    int* mpWidth;
    int* mpHeight;
};


#endif // SIMPLE_RENDERER_H_
