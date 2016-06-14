//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Helper functions for general usage.

#ifndef HELPER_H_
#define HELPER_H_

#include "externals/GLM/glm/glm.hpp"

#include <string>

// Convert color [0..1] to hexadecimal string
std::string RGBAToHexString(glm::vec4 color);

// Shorten URL to minimal  (https://www.google.com -> google.com)
std::string ShortenURL(std::string URL);

// Calculate maximal level of mip map
int MaximalMipMapLevel(int width, int height);

#endif // HELPER_H_
