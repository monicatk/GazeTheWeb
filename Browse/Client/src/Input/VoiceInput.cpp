//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "VoiceInput.h"
#include "src/Utils/Logger.h"

VoiceInput::VoiceInput(eyegui::GUI* pGUI) : _pGUI(pGUI)
{
	// Nothing to do
}

VoiceInput::~VoiceInput()
{
	// Nothing to do
}

bool VoiceInput::StartAudioRecording()
{
	LogInfo("VoiceInput: Start listening");
	return eyegui::startAudioRecording(_pGUI);
}

VoiceAction VoiceInput::EndAndProcessAudioRecording()
{
	// End recording and retrieve audio
	eyegui::endAudioRecording(_pGUI);
	auto spAudio = eyegui::retrieveAudioRecord(_pGUI);
	LogInfo("VoiceInput: End listening");

	// If some audio was retrieved, proceed
	if (spAudio != nullptr)
	{
		// spAudio has different methods to access data like sampleRate, sampleCount and the audio data itself
		LogInfo("VoiceInput: Samples received: ", spAudio->getSampleCount());

		LogInfo("VoiceInput: Start processing");

		// TODO for Voice Input
		// 1. Store Audio locally as .wav (for example in system' TEMP folder)
		//    See this link for "how to save as wave": http://www.cplusplus.com/forum/beginner/166954/
		// 2. Upload wave using CURL
		// 3. Retrieve answer by speech recognition API
		// 4. Parse answer and return action
	}
	else
	{
		LogInfo("VoiceInput: Failure");
	}

	// Fallback if error occurs
	return VoiceAction::NO_ACTION;
}