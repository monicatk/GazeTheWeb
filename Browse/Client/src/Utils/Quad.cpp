//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "Quad.h"
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

Quad::Quad(Type type)
{
	// Decide which data to use
	float const * pQuadVertices = NULL;
	float const * pQuadTextureCoordinates = NULL;
	switch (type)
	{
	case Type::TRIANGLES:
		pQuadVertices = quadTrianglesVertices;
		pQuadTextureCoordinates = quadTrianglesTextureCoordinates;
		mVertexCount = 6;
		break;
	case Type::LINES_WITH_DIAGONAL:
		pQuadVertices = quadLinesWithDiagonalVertices;
		pQuadTextureCoordinates = quadLinesWithDiagonalTextureCoordinates;
		mVertexCount = 10;
		break;
	}

    glGenBuffers(1, &_vbo); // generate VBO
    glBindBuffer(GL_ARRAY_BUFFER, _vbo); // set as current VBO
    glBufferData(GL_ARRAY_BUFFER, mVertexCount * sizeof(float) * 3, pQuadVertices, GL_STATIC_DRAW); // copy data

    glGenBuffers(1, &_uvbo); // generate VBO
    glBindBuffer(GL_ARRAY_BUFFER, _uvbo); // set as current VBO
    glBufferData(GL_ARRAY_BUFFER, mVertexCount * sizeof(float) * 2, pQuadTextureCoordinates, GL_STATIC_DRAW); // copy data
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Quad::~Quad()
{
    glDeleteBuffers(1, &_vbo);
    glDeleteBuffers(1, &_uvbo);
}
