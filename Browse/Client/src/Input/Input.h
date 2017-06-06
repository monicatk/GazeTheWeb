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
		int gazeX,
		int gazeY,
		bool gazeEmulated,
		bool gazeUponGUI,
		bool instantInteraction,
		bool saccade) : 
	gazeX(gazeX),
	gazeY(gazeY),
	gazeEmulated(gazeEmulated),
	gazeUponGUI(gazeUponGUI),
	instantInteraction(instantInteraction),
	saccade(saccade) {}

	// Fields
    double gazeX;
	double gazeY;
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
		int webViewPixelGazeX,
		int webViewPixelGazeY,
		float webViewRelativeGazeX,
		float webViewRelativeGazeY) :

		// TabInput fields
		webViewPixelGazeX(webViewPixelGazeX),
		webViewPixelGazeY(webViewPixelGazeY),
		webViewRelativeGazeX(webViewRelativeGazeX),
		webViewRelativeGazeY(webViewRelativeGazeY),
		insideWebView(
			webViewRelativeGazeX < 1.f
			&& webViewRelativeGazeX >= 0
			&& webViewRelativeGazeY < 1.f
			&& webViewRelativeGazeY >= 0),

		// Fields from input
		gazeX(spInput->gazeX),
		gazeY(spInput->gazeY),
		gazeEmulated(spInput->gazeEmulated),
		gazeUponGUI(spInput->gazeUponGUI),
		instantInteraction(spInput->instantInteraction),
		saccade(spInput->saccade) {}

	// Fields
	int webViewPixelGazeX;
	int webViewPixelGazeY;
	float webViewRelativeGazeX;
	float webViewRelativeGazeY;
	bool insideWebView;

	// Fields from input as reference
	const double& gazeX;
	const double& gazeY;
	const bool& gazeEmulated;
	const bool& gazeUponGUI;
	const bool& instantInteraction;
	const bool& saccade;
};

#endif // INPUT_H_
