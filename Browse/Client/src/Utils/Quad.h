//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Quad to render with OpenGL. Coordinates are from 0 to 1.

#ifndef QUAD_H_
#define QUAD_H_

#include "externals/OGL/gl_core_3_3.h"

class Quad
{
public:

    // Constructor
    Quad();

    // Destructor
    virtual ~Quad();

    // Getter of handles
    GLuint GetVBO() const { return _vbo; }
    GLuint GetUVBO() const { return _uvbo; }

    // Get count of vertices
    int GetVertexCount() const { return 6; }

private:

    // Vertices buffer
    GLuint _vbo = 0;

    // Texture coordinates buffer
    GLuint _uvbo = 0;
};

#endif // QUAD_H_
