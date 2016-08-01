//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Simple wrapper for glm inclusion to get rid of compiler warnings in Visual
// Stuido on Windows.

#ifdef _WIN32
#pragma warning( push )
#pragma warning( disable : 4201)
#endif

#include "submodules/glm/glm/glm.hpp"

#ifdef _WIN32
#pragma warning( pop )
#endif