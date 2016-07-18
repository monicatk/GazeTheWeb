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
	// default constructor
	Rect()
	{
		top = 0.f;
		left = 0.f;
		bottom = 0.f;
		right = 0.f;
	};

	Rect(float t, float l, float b, float r)
	{
		top = t;
		left = l;
		bottom = b;
		right = r;
	};

	// NOTE: I left out the members' underscores, due to simplicity in accessing them (public)
	float top;
	float left;
	float bottom;
	float right;
	
	float width() const { return right - left; };
	float height() const { return top - bottom; };
	glm::vec2 center() const { return glm::vec2(left + width() / 2, top + height() / 2); };
};

#endif