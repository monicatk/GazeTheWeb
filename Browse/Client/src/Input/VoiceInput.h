//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Stub for voice input handling.

#ifndef VOICEINPUT_H_
#define VOICEINPUT_H_

#include "submodules/eyeGUI/include/eyeGUI.h"
#include "submodules/json/src/json.hpp"
using json = nlohmann::json;

enum class VoiceAction
{
	NO_ACTION, SCROLL_UP, SCROLL_DOWN,
	BOOKMARK, BACK, FORWARD, BOTTOM,
	TOP, RELOAD,GO_TO,SEARCH,TEXT_INPUT,VIDEO_INPUT,
	CLICK,INCREASE,DECREASE,MUTE,UNMUTE,PLAY,STOP,JUMP, NEW_TAB,QUIT

};

// PROPOSAL TO GET MORE THAN AN ACTION OUT OF IT
struct VoiceResult
{
	// Constructor
	VoiceResult(VoiceAction action, std::string keyworkds)
	{
		this->action = action;
		this->keyworkds = keyworkds;
	}

	bool IsSomething() { return action != VoiceAction::NO_ACTION; }

	VoiceAction action;
	std::string keyworkds;
};

class VoiceInput
{
public:

	// Constructor
	VoiceInput(eyegui::GUI* pGUI); // TODO FOR LATER: Avoid handing over GUI pointer but make more elaborated solution

								   // Destructor
	virtual ~VoiceInput();

	// Method to start recording, returns whether successful (must be manually triggered right now)
	bool StartAudioRecording();

	// Method to end recording and do processing. Returns action for browser to perform. 
	VoiceResult EndAndProcessAudioRecording();

private:

	// Members
	eyegui::GUI* _pGUI;
};

#endif // VOICEINPUT_H_