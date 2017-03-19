//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// RenderItem with primitive as mesh.

#ifndef PRIMITIVERENDERITEM_H_
#define PRIMITIVERENDERITEM_H_

#include "src/Utils/RenderItem.h"
#include "src/Utils/Primitive.h"

class PrimitiveRenderItem : public RenderItem
{
public:

    // Constructors
	PrimitiveRenderItem(std::string vertSource, std::string fragSource, Primitive::Type type = Primitive::Type::QUAD_TRIANGLES);
	PrimitiveRenderItem(std::string vertSource, std::string geomSource, std::string fragSource, Primitive::Type type = Primitive::Type::QUAD_TRIANGLES);

    // Drawing
    virtual void Draw(GLenum mode = GL_TRIANGLES) const;

protected:

    // Mesh
    std::unique_ptr<Primitive> _upPrimitve;

private:

    // Initialization of primitive
    void InitPrimitive(Primitive::Type type);
};

#endif // PRIMITIVERENDERITEM_H_
