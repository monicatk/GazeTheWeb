//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Mueller (muellerd@uni-koblenz.de)
//============================================================================
// Rectangle structure used for bounding box of DOM nodes.

#ifndef RECT_H_
#define RECT_H_

#include "src/Utils/glmWrapper.h"
#include <vector>
#include <string>

class Rect
{
public:

	// Default constructor
	Rect()
	{
		top = 0;
		left = 0;
		bottom = 0;
		right = 0;
	}

	// Constructor taking single values
	Rect(float t, float l, float b, float r)
	{
		top = t;
		left = l;
		bottom = b;
		right = r;
	}

	// Constructor taking vector of values
	Rect(std::vector<float> data)
	{
		// Simulate default constructor if not enough data is given
		if (data.size() < 4)
		{
			data.clear();
			data = { 0.f, 0.f, 0.f, 0.f };
		}

		top = data[0];
		left = data[1];
		bottom = data[2];
		right = data[3];
	}

	// Public available members
	float top = 0.f;
	float left = 0.f;
	float bottom = 0.f;
	float right = 0.f;

	// Methods
	float Width() const { return right - left; }
	float Height() const { return bottom - top; }

	glm::vec2 Center() const { return glm::vec2(left + (Width() / 2.f), top + (Height() / 2.f)); }

	bool IsZero() const { return (Width() <= 0 || Height() <= 0); }

	std::string ToString() const
	{
		return "(" + std::to_string(top) + ", " + std::to_string(left) + ", "
			+ std::to_string(bottom) + ", " + std::to_string(right) + ")";
	}

	bool IsInside(float x, float y) const
	{
		return y <= bottom && y >= top && x >= left && x <= right;
	}
};

#endif