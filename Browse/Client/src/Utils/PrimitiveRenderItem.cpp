//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "PrimitiveRenderItem.h"

PrimitiveRenderItem::PrimitiveRenderItem(std::string vertSource, std::string fragSource, Primitive::Type type)
: RenderItem(vertSource, fragSource)
{
	InitPrimitive(type);
}

PrimitiveRenderItem::PrimitiveRenderItem(std::string vertSource, std::string geomSource, std::string fragSource, Primitive::Type type)
: RenderItem(vertSource, geomSource, fragSource)
{
	InitPrimitive(type);
}

void PrimitiveRenderItem::Draw(GLenum mode) const
{
    glDrawArrays(mode, 0, _upPrimitve->GetVertexCount());
}

void PrimitiveRenderItem::InitPrimitive(Primitive::Type type)
{
    // Create mesh
	_upPrimitve = std::unique_ptr<Primitive>(new Primitive(type));

    // Bring both with vertex array buffer together
    glBindVertexArray(_vao);

    // Vertices
    int posAttr = glGetAttribLocation(_upShader->GetProgram(), "posAttr");
    glEnableVertexAttribArray(posAttr);
    glBindBuffer(GL_ARRAY_BUFFER, _upPrimitve->GetVBO());
    glVertexAttribPointer(posAttr, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    // Texture coordinates
    int uvAttrib = glGetAttribLocation(_upShader->GetProgram(), "uvAttr");
    glEnableVertexAttribArray(uvAttrib);
    glBindBuffer(GL_ARRAY_BUFFER, _upPrimitve->GetUVBO());
    glVertexAttribPointer(uvAttrib, 2, GL_FLOAT, GL_FALSE, 0, NULL);

    // Unbind everything
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
