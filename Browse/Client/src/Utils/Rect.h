//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Mueller (muellerd@uni-koblenz.de)
//============================================================================

#ifndef RECT_H_
#define RECT_H_

#include "submodules/glm/glm/glm.hpp"
#include <vector>

class Rect
{
public:

	// Default constructor
	Rect() { 
		top = 0; 
		left = 0;
		bottom = 0;
		right = 0;
	}

	// Constructor
	Rect(float t, float l, float b, float r)
	{
		top = t;
		left = l;
		bottom = b;
		right = r;
	}

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
	float width() const { return right - left; }
	float height() const { return bottom - top; }
	glm::vec2 center() const { return glm::vec2(left + (width() / 2.f), top + (height() / 2.f)); }

	bool isZero() const { return (width() && height()); }
};

#endif