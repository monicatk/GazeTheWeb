//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Mueller (muellerd@uni-koblenz.de)
//============================================================================

#ifndef RECT_H_
#define RECT_H_

#include "submodules/glm/glm/glm.hpp"

class Rect
{
public:

	// Default constructor
	Rect() {}

	// Constructor
	Rect(float t, float l, float b, float r)
	{
		top = t;
		left = l;
		bottom = b;
		right = r;
	}

	// Public available members
	float top = 0.f;
	float left = 0.f;
	float bottom = 0.f;
	float right = 0.f;

	// Methods
	float width() const { return right - left; }
	float height() const { return bottom - top; }
	glm::vec2 center() const { return glm::vec2(left + (width() / 2.f), top + (height() / 2.f)); }
};

#endif