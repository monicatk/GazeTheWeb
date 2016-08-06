//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// RenderItem which just draws a point but can be manipulated shaders.
// TODO: Draw call is strange because here in base class only GL_POINTS make
// sense but in other class QuadRenderItem may take different.

#ifndef RENDERITEM_H_
#define RENDERITEM_H_

#include "src/Utils/Shader.h"
#include "src/Utils/Quad.h"
#include <memory>

class RenderItem
{
public:

    // Constructors
    RenderItem(std::string vertSource, std::string fragSource);
    RenderItem(std::string vertSource, std::string geomSource, std::string fragSource);

    // Destructor
    virtual ~RenderItem();

    // Binding for updating values and drawing
    void Bind() const;

    // Drawing
    virtual void Draw(GLenum mode = GL_TRIANGLES) const;

    // Getter of shader for updating values etc.
    Shader const * GetShader() const { return _upShader.get(); }

protected:

    // Shader
    std::unique_ptr<Shader> _upShader;

    // Vertex Array Object handle
    GLuint _vao;
};

#endif // RENDERITEM_H_
