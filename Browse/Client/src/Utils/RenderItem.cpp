//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "RenderItem.h"

RenderItem::RenderItem(std::string vertSource, std::string fragSource)
{
    // Create shader
    _upShader = std::unique_ptr<Shader>(new Shader(vertSource, fragSource));

    // Just create a vertex array buffer
    glGenVertexArrays(1, &_vao);
}

RenderItem::RenderItem(std::string vertSource, std::string geomSource, std::string fragSource)
{
    // Create shader
    _upShader = std::unique_ptr<Shader>(new Shader(vertSource, geomSource, fragSource));

    // Just create a vertex array buffer
    glGenVertexArrays(1, &_vao);
}

RenderItem::~RenderItem()
{
    glDeleteVertexArrays(1, &_vao);
}

void RenderItem::Bind() const
{
    _upShader->Bind();
    glBindVertexArray(_vao);
}

void RenderItem::Draw(GLenum mode) const
{
    glDrawArrays(mode, 0, 1);
}

