//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Framebuffer for post processing.

#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "externals/OGL/gl_core_3_3.h"
#include "src/Utils/glmWrapper.h"
#include <vector>
#include <stack>

class Framebuffer
{
public:

    // Enumeration of color formats
    enum ColorFormat
    {
        RGB, RGBA
    };

    // Constructor
    Framebuffer(int width, int height);

    // Destructor
    virtual ~Framebuffer();

    // Bind framebuffer
    void Bind() const;

    // Unbind (binds default, as long as it is zero)
    void Unbind() const;

    // Resizing (needs bound framebuffer)
    void Resize(int width, int height);

    // Add attachment (needs bound framebuffer)
	void AddAttachment(ColorFormat colorFormat, bool clampToBorder = false);

    // Get texture handle of color attachment
    GLuint GetAttachment(int number) const { return _colorAttachments.at(number).first; }

private:

    // Static stack for framebuffer handles which is used by bind and unbind
    static std::stack<GLuint> _handleStack;

    // Pair of color format and texture handle
    typedef std::pair<GLuint, ColorFormat> ColorAttachment;

    // Members
    std::vector<ColorAttachment> _colorAttachments;
    GLuint _framebuffer;
    GLuint _depthStencil;
    int _width = -1;
    int _height = -1;
};

#endif // FRAMEBUFFER_H
