//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Tab interface for debugging overlays. Does not control, whether any global
// variable is set for showing / hiding debugging overlay.

#ifndef TABDEBUGGINGINTERFACE_H_
#define TABDEBUGGINGINTERFACE_H_

class TabDebuggingInterface
{
public:

	// Size in pixels, coordinate system is WebViewPixel. Rectangle centered around coordinate
	virtual void Debug_DrawRectangle(glm::vec2 coordinate, glm::vec2 size, glm::vec3 color) const = 0;

	// Coordinate system is WebViewPixel
	virtual void Debug_DrawLine(glm::vec2 originCoordinate, glm::vec2 targetCoordinate, glm::vec3 color) const = 0;
};

#endif // TABDEBUGGINGINTERFACE_H_
