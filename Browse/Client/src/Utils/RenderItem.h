//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// RenderItems combines shaders with meshes.

#ifndef RENDERITEM_H_
#define RENDERITEM_H_

#include "src/Utils/Shader.h"
#include "src/Utils/Quad.h"
#include <memory>

class RenderItem
{
public:

    // Constructor
    RenderItem(std::string vertSource, std::string fragSource);

    // Destructor
    virtual ~RenderItem();

    // Binding for updating values and drawing
    void Bind() const;

    // Drawing
    void Draw(GLenum mode = GL_TRIANGLES) const;

    // Getter of shader for updating values etc.
    Shader const * GetShader() const { return _upShader.get(); }

private:

    // Shader
    std::unique_ptr<Shader> _upShader;

    // Mesh
    std::unique_ptr<Quad> _upQuad;

    // Vertex Array Object handle
    GLuint _vao;
};

#endif // RENDERITEM_H_
