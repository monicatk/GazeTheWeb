//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Input struct. Origin in upper left display corner.

#ifndef INPUT_H_
#define INPUT_H_

#include <memory>

class Input
{
public:

    Input(
		float gazeX,
		float gazeY,
		float rawGazeX,
		float rawGazeY,
		float gazeAge,
		bool gazeEmulated,
		bool gazeUponGUI,
		bool instantInteraction,
		bool saccade) : 
	gazeX(gazeX),
	gazeY(gazeY),
	rawGazeX(rawGazeX),
	rawGazeY(rawGazeY),
	gazeAge(gazeAge),
	gazeEmulated(gazeEmulated),
	gazeUponGUI(gazeUponGUI),
	instantInteraction(instantInteraction),
	saccade(saccade) {}

	// Fields
    float gazeX;
	float gazeY;
	float rawGazeX;
	float rawGazeY;
	float gazeAge;
	bool gazeEmulated;
    bool gazeUponGUI;
	bool instantInteraction;
	bool saccade; // indicator whether current gaze is classified as part of a saccade
};

class TabInput
{
public:
	TabInput(
		const std::shared_ptr<const Input> spInput,
		int webViewX,
		int webViewY,
		int webViewWidth,
		int webViewHeight,
		int webViewResolutionX,
		int webViewResolutionY) :

		// Fields from input
		gazeX(spInput->gazeX),
		gazeY(spInput->gazeY),
		rawGazeX(spInput->rawGazeX),
		rawGazeY(spInput->rawGazeY),
		gazeAge(spInput->gazeAge),
		gazeEmulated(spInput->gazeEmulated),
		gazeUponGUI(spInput->gazeUponGUI),
		instantInteraction(spInput->instantInteraction),
		saccade(spInput->saccade),

		// TabInput fields
		webViewPixelGazeX(spInput->gazeX - (float)webViewX),
		webViewPixelGazeY(spInput->gazeY - (float)webViewY),
		webViewRelativeGazeX(webViewPixelGazeX / (float)webViewWidth),
		webViewRelativeGazeY(webViewPixelGazeY / (float)webViewHeight),
		CEFPixelGazeX(webViewRelativeGazeX * (float)webViewResolutionX),
		CEFPixelGazeY(webViewRelativeGazeY * (float)webViewResolutionY),
		insideWebView(
			webViewRelativeGazeX < 1.f
			&& webViewRelativeGazeX >= 0
			&& webViewRelativeGazeY < 1.f
			&& webViewRelativeGazeY >= 0)
		{}

	// Fields
	float webViewPixelGazeX;
	float webViewPixelGazeY;
	float webViewRelativeGazeX;
	float webViewRelativeGazeY;
	float CEFPixelGazeX;
	float CEFPixelGazeY;
	bool insideWebView;

	// Fields from input as reference
	const float& gazeX;
	const float& gazeY;
	const float& rawGazeX;
	const float& rawGazeY;
	const float& gazeAge;
	const bool& gazeEmulated;
	const bool& gazeUponGUI;
	const bool& instantInteraction;
	const bool& saccade;
};

#endif // INPUT_H_
