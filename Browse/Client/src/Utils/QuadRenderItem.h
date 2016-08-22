//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// RenderItem with quad as mesh.

#ifndef QUADRENDERITEM_H_
#define QUADRENDERITEM_H_

#include "src/Utils/RenderItem.h"

class QuadRenderItem : public RenderItem
{
public:

    // Constructors
    QuadRenderItem(std::string vertSource, std::string fragSource, Quad::Type type = Quad::Type::TRIANGLES);
    QuadRenderItem(std::string vertSource, std::string geomSource, std::string fragSource, Quad::Type type = Quad::Type::TRIANGLES);

    // Drawing
    virtual void Draw(GLenum mode = GL_TRIANGLES) const;

protected:

    // Mesh
    std::unique_ptr<Quad> _upQuad;

private:

    // Initialization of quad
    void InitQuad(Quad::Type type);
};

#endif // QUADRENDERITEM_H_
