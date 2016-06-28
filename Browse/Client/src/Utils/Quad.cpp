//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "Quad.h"
#include <vector>

// Geometry
const float quadVertices[] =
{
    0,0,0, 1,0,0, 1,1,0,
    1,1,0, 0,1,0, 0,0,0
};

const float quadTextureCoordinates[] =
{
    0,0, 1,0, 1,1,
    1,1, 0,1, 0,0
};

Quad::Quad()
{
    glGenBuffers(1, &_vbo); // generate VBO
    glBindBuffer(GL_ARRAY_BUFFER, _vbo); // set as current VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW); // copy data

    glGenBuffers(1, &_uvbo); // generate VBO
    glBindBuffer(GL_ARRAY_BUFFER, _uvbo); // set as current VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadTextureCoordinates), quadTextureCoordinates, GL_STATIC_DRAW); // copy data
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Quad::~Quad()
{
    glDeleteBuffers(1, &_vbo);
    glDeleteBuffers(1, &_uvbo);
}
