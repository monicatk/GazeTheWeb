//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "Framebuffer.h"

// Define stack
std::stack<GLuint> Framebuffer::_handleStack;

Framebuffer::Framebuffer(int width, int height)
{
    // Generate framebuffer and renderbuffer for depth and stencil tests
    glGenFramebuffers(1, &_framebuffer);
    glGenRenderbuffers(1, &_depthStencil);

    // Bind framebuffer
    Bind();

    // Create render buffer for depth and stencil and save width and height
    Resize(width, height);

    // Unbind framebuffer
    Unbind();
}

Framebuffer::~Framebuffer()
{
    glDeleteFramebuffers(1, &_framebuffer);
    glDeleteRenderbuffers(1, &_depthStencil);

    for(const auto& rPair : _colorAttachments)
    {
        glDeleteTextures(1, &rPair.first);
    }
}

void Framebuffer::Bind() const
{
    if(_handleStack.empty() || (_handleStack.top() != _framebuffer))
    {
        _handleStack.push(_framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);
    }
}

void Framebuffer::Unbind() const
{
    // Check for empty stack
    if(_handleStack.empty())
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    else
    {
        // Check whether this is on top
        if(_handleStack.top() == _framebuffer)
        {
            _handleStack.pop();

            // Decide whether to bind fallback or next on stack
            if(_handleStack.empty())
            {
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
            }
            else
            {
                glBindFramebuffer(GL_FRAMEBUFFER, _handleStack.top());
            }
        }
        else
        {
            // Do nothing about it
        }
    }
}

void Framebuffer::Resize(int width, int height)
{
	if (width != _width || height != _height)
	{
		// Save width and height
		_width = width;
		_height = height;

		// Create renderbuffer
		glBindRenderbuffer(GL_RENDERBUFFER, _depthStencil);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _width, _height);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		// (Re)Bind depth and stencil
		glFramebufferRenderbuffer(
			GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _depthStencil);

		// Do it for all color attachments
		for (const auto& rPair : _colorAttachments)
		{
			glBindTexture(GL_TEXTURE_2D, rPair.first);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				rPair.second == ColorFormat::RGB ? GL_RGB8 : GL_RGBA8,
				_width,
				_height,
				0,
				rPair.second == ColorFormat::RGB ? GL_RGB : GL_RGBA,
				GL_UNSIGNED_BYTE,
				NULL);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}
}

void Framebuffer::AddAttachment(ColorFormat colorFormat, bool clampToBorder)
{
    // Generate new texture
    GLuint texture = 0;
    glGenTextures(1, &texture);

    // Bind texture and set it up
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, clampToBorder ? GL_CLAMP_TO_BORDER : GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, clampToBorder ? GL_CLAMP_TO_BORDER : GL_CLAMP_TO_EDGE);

    // Reserve space for texture
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        colorFormat == ColorFormat::RGB ? GL_RGB8 : GL_RGBA8,
        _width,
        _height,
        0,
        colorFormat == ColorFormat::RGB ? GL_RGB : GL_RGBA,
        GL_UNSIGNED_BYTE,
        NULL);

    // Unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);

    // Find out which color attachment should be used
    int count = _colorAttachments.size();
    GLenum attachmentNumber = GL_COLOR_ATTACHMENT0;
    attachmentNumber += count;

    // Bind texture to framebuffer
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, attachmentNumber, GL_TEXTURE_2D, texture, 0);

    // Tell framebuffer how many attachments to fill
    std::vector<GLenum> attachmentIdentifiers;
    for(int i = 0; i <= count; i++)
    {
        attachmentIdentifiers.push_back(GL_COLOR_ATTACHMENT0 + i);
    }
    glDrawBuffers(attachmentIdentifiers.size(), attachmentIdentifiers.data());

    // Remember that attachment
    _colorAttachments.push_back(std::make_pair(texture, colorFormat));
}
