//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "Primitive.h"
#include <vector>

// Geometry
const float quadTrianglesVertices[] =
{
    0,0,0, 1,0,0, 1,1,0,
    1,1,0, 0,1,0, 0,0,0
};

const float quadTrianglesTextureCoordinates[] =
{
    0,0, 1,0, 1,1,
    1,1, 0,1, 0,0
};

const float quadLinesWithDiagonalVertices[] =
{
	0,0,0, 1,0,0,
	1,0,0, 1,1,0,
	1,1,0, 0,1,0,
	0,1,0, 0,0,0,
	0,1,0, 1,0,0
};

const float quadLinesWithDiagonalTextureCoordinates[] =
{
	0,0, 1,0,
	1,0, 1,1,
	1,1, 0,1,
	0,1, 0,0,
	0,1, 1,0,
};

const float lineVertices[] =
{
	0,0,0, 1,0,0
};

const float lineTextureCoordinates[] =
{
	0,0, 1,0
};


Primitive::Primitive(Type type)
{
	// Decide which data to use
	float const * pVertices = NULL;
	float const * pTextureCoordinates = NULL;
	switch (type)
	{
	case Type::QUAD_TRIANGLES:
		pVertices = quadTrianglesVertices;
		pTextureCoordinates = quadTrianglesTextureCoordinates;
		mVertexCount = 6;
		break;
	case Type::QUAD_LINES_WITH_DIAGONAL:
		pVertices = quadLinesWithDiagonalVertices;
		pTextureCoordinates = quadLinesWithDiagonalTextureCoordinates;
		mVertexCount = 10;
		break;
	case Type::LINE:
		pVertices = lineVertices;
		pTextureCoordinates = lineTextureCoordinates;
		mVertexCount = 2;
		break;
	}

    glGenBuffers(1, &_vbo); // generate VBO
    glBindBuffer(GL_ARRAY_BUFFER, _vbo); // set as current VBO
    glBufferData(GL_ARRAY_BUFFER, mVertexCount * sizeof(float) * 3, pVertices, GL_STATIC_DRAW); // copy data

    glGenBuffers(1, &_uvbo); // generate VBO
    glBindBuffer(GL_ARRAY_BUFFER, _uvbo); // set as current VBO
    glBufferData(GL_ARRAY_BUFFER, mVertexCount * sizeof(float) * 2, pTextureCoordinates, GL_STATIC_DRAW); // copy data
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Primitive::~Primitive()
{
    glDeleteBuffers(1, &_vbo);
    glDeleteBuffers(1, &_uvbo);
}
