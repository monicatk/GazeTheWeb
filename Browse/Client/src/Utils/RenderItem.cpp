//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "RenderItem.h"

RenderItem::RenderItem(std::string vertSource, std::string fragSource)
{
    // Create shader
    _upShader = std::unique_ptr<Shader>(new Shader(vertSource, fragSource));

    // Create mesh
    _upQuad = std::unique_ptr<Quad>(new Quad);

    // Bring both with vertex array buffer together
    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

    // Vertices
    int posAttr = glGetAttribLocation(_upShader->getProgram(), "posAttr");
    glEnableVertexAttribArray(posAttr);
    glBindBuffer(GL_ARRAY_BUFFER, _upQuad->GetVBO());
    glVertexAttribPointer(posAttr, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    // Texture coordinates
    int uvAttrib = glGetAttribLocation(_upShader->getProgram(), "uvAttr");
    glEnableVertexAttribArray(uvAttrib);
    glBindBuffer(GL_ARRAY_BUFFER, _upQuad->GetUVBO());
    glVertexAttribPointer(uvAttrib, 2, GL_FLOAT, GL_FALSE, 0, NULL);

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
    glDrawArrays(mode, 0, _upQuad->GetVertexCount());
}

