//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Parameters of WebView which may be set by Tab or Action.

#ifndef WEBVIEWPARAMETERS_H_
#define WEBVIEWPARAMETERS_H_

#include "src/Utils/glmWrapper.h"

struct WebViewParameters
{
    glm::vec2 centerOffset = glm::vec2(0, 0);
    glm::vec2 zoomPosition = glm::vec2(0, 0);
    float zoom = 1.f;
    float dim = 0.f;
    float highlight = 0.f;
};

#endif // WEBVIEWPARAMETERS_H_
