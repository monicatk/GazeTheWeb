//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "QuadRenderItem.h"

QuadRenderItem::QuadRenderItem(std::string vertSource, std::string fragSource, Quad::Type type)
: RenderItem(vertSource, fragSource)
{
    InitQuad(type);
}

QuadRenderItem::QuadRenderItem(std::string vertSource, std::string geomSource, std::string fragSource, Quad::Type type)
: RenderItem(vertSource, geomSource, fragSource)
{
    InitQuad(type);
}

void QuadRenderItem::Draw(GLenum mode) const
{
    glDrawArrays(mode, 0, _upQuad->GetVertexCount());
}

void QuadRenderItem::InitQuad(Quad::Type type)
{
    // Create mesh
    _upQuad = std::unique_ptr<Quad>(new Quad(type));

    // Bring both with vertex array buffer together
    glBindVertexArray(_vao);

    // Vertices
    int posAttr = glGetAttribLocation(_upShader->GetProgram(), "posAttr");
    glEnableVertexAttribArray(posAttr);
    glBindBuffer(GL_ARRAY_BUFFER, _upQuad->GetVBO());
    glVertexAttribPointer(posAttr, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    // Texture coordinates
    int uvAttrib = glGetAttribLocation(_upShader->GetProgram(), "uvAttr");
    glEnableVertexAttribArray(uvAttrib);
    glBindBuffer(GL_ARRAY_BUFFER, _upQuad->GetUVBO());
    glVertexAttribPointer(uvAttrib, 2, GL_FLOAT, GL_FALSE, 0, NULL);

    // Unbind everything
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
