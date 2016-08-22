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

	// Enumeration for different quad types
	enum class Type { TRIANGLES, LINES_WITH_DIAGONAL };

    // Constructor
    Quad(Type type = Type::TRIANGLES);

    // Destructor
    virtual ~Quad();

    // Getter of handles
    GLuint GetVBO() const { return _vbo; }
    GLuint GetUVBO() const { return _uvbo; }

    // Get count of vertices
    int GetVertexCount() const { return mVertexCount; }

private:

    // Vertices buffer
    GLuint _vbo = 0;

    // Texture coordinates buffer
    GLuint _uvbo = 0;

	// Count of vertices
	int mVertexCount = 0;
};

#endif // QUAD_H_
