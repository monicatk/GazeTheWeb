//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Mueller (muellerd@uni-koblenz.de)
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "src/State/Web/Tab/Tab.h"
#include "src/Master.h"

void Tab::CalculateWebViewPositionAndSize(int& rX, int& rY, int& rWidth, int& rHeight) const
{
	auto webViewCoordinates = CalculateWebViewPositionAndSize();
	rX = webViewCoordinates.x;
	rY = webViewCoordinates.y;
	rWidth = webViewCoordinates.width;
	rHeight = webViewCoordinates.height;
}

void Tab::GetWindowSize(int& rWidth, int& rHeight) const
{
	rWidth = _pMaster->GetWindowWidth();
	rHeight = _pMaster->GetWindowHeight();
}