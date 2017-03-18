//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Primitive to render with OpenGL. Coordinates are from 0 to 1.

#ifndef PRIMITIVE_H_
#define PRIMITIVE_H_

#include "externals/OGL/gl_core_3_3.h"

class Primitive
{
public:

	// Enumeration for different primitive types
	enum class Type { QUAD_TRIANGLES, QUAD_LINES_WITH_DIAGONAL, LINE };

    // Constructor
	Primitive(Type type = Type::QUAD_TRIANGLES);

    // Destructor
    virtual ~Primitive();

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

#endif // PRIMITIVE_H_
