//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================


#include "VoiceInput.h"
#include <cmath>
#include <fstream>
#include "src/Utils/Logger.h"
#include <iostream>
#include <map> 
#include <set> 
#include <regex>
#include "externals/curl/include/curl/curl.h"
#include "externals/base64/base64.h"

//voice script to actions, able to mishear
std::map<std::string, VoiceAction> voiceActionMapping = {

	{ "up", VoiceAction::SCROLL_UP },
	{ "app", VoiceAction::SCROLL_UP },
	{ "down", VoiceAction::SCROLL_DOWN },
	{ "town", VoiceAction::SCROLL_DOWN },
	{ "dawn", VoiceAction::SCROLL_DOWN },
	{ "dumb", VoiceAction::SCROLL_DOWN },
	{ "bookmark", VoiceAction::BOOKMARK },
	{ "back", VoiceAction::BACK },
	{ "top", VoiceAction::TOP },
	{ "bottom", VoiceAction::BOTTOM },
	{ "button", VoiceAction::BOTTOM },
	{ "reload", VoiceAction::RELOAD },
	{ "forward", VoiceAction::FORWARD },
	{ "text", VoiceAction::TEXT_INPUT },
	{ "video", VoiceAction::VIDEO_INPUT },
	{ "go to", VoiceAction::GO_TO },
	{ "type", VoiceAction::SEARCH },
	{ "click", VoiceAction::CLICK },
	{ "clique", VoiceAction::CLICK },
	{ "clip", VoiceAction::CLICK },
	{ "please", VoiceAction::CLICK },
	{"increase",VoiceAction::INCREASE},
	{ "decrease",VoiceAction::DECREASE },
	{ "mute",VoiceAction::MUTE },
	{ "unmute",VoiceAction::UNMUTE },
	{ "play",VoiceAction::PLAY },
	{ "crypt",VoiceAction::QUIT },
	{ "jump",VoiceAction::JUMP },
	{ "drum",VoiceAction::JUMP },
	{ "quit",VoiceAction::QUIT },
	{ "new tab",VoiceAction::NEW_TAB },
	{ "utep",VoiceAction::NEW_TAB },
	{ "neutab",VoiceAction::NEW_TAB },
	{ "neo tap",VoiceAction::NEW_TAB },
<<<<<<< HEAD
	{ "checkbox",VoiceAction::CHECKBOX },
	{ "delete",VoiceAction::DELETEALL },
	{ "backspace",VoiceAction::BACKSPACE }
=======
	{ "check",VoiceAction::CHECKBOX },
	{ "chuck",VoiceAction::CHECKBOX },
>>>>>>> 2c468da9488a36963d5edcc8dcb881eb331bf19e
	//{ "",VoiceAction:: },

};
std::set<std::string> voiceActionKeys = {
	"up","app",
	"down" , "town","dumb","dawn","drum",
	"bookmark","back", "top","new tab","neo tap","neutab","utep",
	"reload","bottom","button","forward",
	"text","video",
	"go to","type","click","clique","clip","please",
	"increase","decrease",
	"mute","unmute",
<<<<<<< HEAD
	"play","jump","quit","crypt","checkbox",
	"delete","backspace"
=======
	"play","jump","quit","crypt","check","chuck"
>>>>>>> 2c468da9488a36963d5edcc8dcb881eb331bf19e
};

std::map<std::string, std::string> textToDigit = {
	{"zero","0"}, { "one","1"},{"juan","1"}, { "two","2" },{ "to","2" },
	{ "three","3" },{ "four","4" },	{ "for","4" },
	{ "five","5" },{ "six","6" },{ "sex","6" },
	{ "seven","7" },{ "eight","8" },{ "att","8" },
	{ "nine","9" },{ "ten","10" },{ "tan","10" }


};
std::set<std::string> textToDigitKeyKeys = {
	"zero","one","two","three","four","five","six","seven","eight","nine","ten",
	"juan","to","for","att","sex","tan"
};

//deal with para
std::string findPrefixAndParameters(VoiceAction action, std::vector<std::string> transcript, int paraIndex) {
	if (action == VoiceAction::GO_TO || action == VoiceAction::NEW_TAB)
		// command GO TO
	{
		std::string urlString = "";
		if (transcript.size() > 1) {
			if (!transcript[paraIndex].empty()) {
				urlString = transcript[paraIndex];
				size_t found = urlString.find('.');
				if (found == std::string::npos) {
					urlString = transcript[paraIndex] + ".com";
				}
				return urlString;
			}

		}
	}
	//command SEARCH
	if (action == VoiceAction::SEARCH || action == VoiceAction::CLICK) {
		if (transcript.size() > 1) {
			if (!transcript[paraIndex].empty()) {
				std::string searchText = "";
				for (int i = paraIndex; i < transcript.size(); i++) { searchText += transcript[i]; };
				return searchText;
			}
		}
	}
	if (action == VoiceAction::TEXT_INPUT || action == VoiceAction::VIDEO_INPUT || action == VoiceAction::JUMP) {
		if (transcript.size() > 1) {
			if (!transcript[paraIndex].empty()) {
				std::string num = "";
				size_t last_index = transcript[paraIndex].find_last_not_of("0123456789");
				num = transcript[paraIndex].substr(last_index + 1);
				if (num.empty()) {
					for (std::string key : textToDigitKeyKeys) {
						std::size_t found = transcript[paraIndex].find(key);
						if (found != std::string::npos)
							num += textToDigit[key];
					}
				}
				return num;
			}
		}
	}

	return "";
}

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
	LogInfo("VoiceInput: -----------Start recording voice--------------");
	return eyegui::startAudioRecording(_pGUI);
}

// levenshtein distance
size_t uiLevenshteinDistance(const std::string &s1, const std::string &s2)
{
	const size_t m(s1.size());
	const size_t n(s2.size());

	if (m == 0) return n;
	if (n == 0) return m;

	size_t *costs = new size_t[n + 1];

	for (size_t k = 0; k <= n; k++) costs[k] = k;

	size_t i = 0;
	for (std::string::const_iterator it1 = s1.begin(); it1 != s1.end(); ++it1, ++i)
	{
		costs[0] = i + 1;
		size_t corner = i;

		size_t j = 0;
		for (std::string::const_iterator it2 = s2.begin(); it2 != s2.end(); ++it2, ++j)
		{
			size_t upper = costs[j + 1];
			if (*it1 == *it2)
			{
				costs[j + 1] = corner;
			}
			else
			{
				size_t t(upper < corner ? upper : corner);
				costs[j + 1] = (costs[j] < t ? costs[j] : t) + 1;
			}

			corner = upper;
		}
	}

	size_t result = costs[n];
	delete[] costs;

	return result;
}

//split a string
template<typename Out>
void split(const std::string &s, char delim, Out result) {
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		*(result++) = item;
	}
}
std::vector<std::string> split(const std::string &s, char delim) {
	std::vector<std::string> elems;
	split(s, delim, std::back_inserter(elems));
	return elems;
}

size_t WriteCallbackVoice(void *contents, size_t size, size_t nmemb, void *userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}


struct wavfile_header {
	char	riff_tag[4];
	int		riff_length;
	char	wave_tag[4];
	char	fmt_tag[4];
	int		fmt_length;
	short	audio_format;
	short	num_channels;
	int		sample_rate;
	int		byte_rate;
	short	block_align;
	short	bits_per_sample;
	char	data_tag[4];
	int		data_length;
};

FILE * wavfile_open(const char *filename, int samples_per_second, short num_channels)
{
	struct wavfile_header header;
	int bits_per_sample = 16;

	strncpy(header.riff_tag, "RIFF", 4);
	strncpy(header.wave_tag, "WAVE", 4);
	strncpy(header.fmt_tag, "fmt ", 4);
	strncpy(header.data_tag, "data", 4);

	header.riff_length = 0;
	header.fmt_length = 16;
	header.audio_format = 1;
	header.num_channels = num_channels;
	header.sample_rate = samples_per_second;
	header.byte_rate = samples_per_second*(bits_per_sample / 8);
	header.block_align = bits_per_sample / 8;
	header.bits_per_sample = bits_per_sample;
	header.data_length = 0;

	FILE * file = fopen(filename, "wb+");

	fwrite(&header, sizeof(header), 1, file);

	fflush(file);

	return file;

}

void wavfile_write(FILE *file, const short data[], int length)
{
	fwrite(data, sizeof(short), length, file);
}

void wavfile_close(FILE *file)
{
	int file_length = ftell(file);
	LogInfo("VoiceInput: file_length: ", file_length);
	int data_length = file_length - sizeof(struct wavfile_header);
	fseek(file, sizeof(struct wavfile_header) - sizeof(int), SEEK_SET);
	fwrite(&data_length, sizeof(data_length), 1, file);

	int riff_length = file_length - 8;
	fseek(file, 4, SEEK_SET);
	fwrite(&riff_length, sizeof(riff_length), 1, file);

	fclose(file);
}

VoiceResult VoiceInput::EndAndProcessAudioRecording()
{
	VoiceResult voiceResult(VoiceAction::NO_ACTION, "");
	// End recording and retrieve audio
	eyegui::endAudioRecording(_pGUI);
	auto spAudio = eyegui::retrieveAudioRecord(_pGUI);
	LogInfo("VoiceInput: ------- End recording,send to api now ------");

	// If some audio was retrieved, proceed
	if (spAudio != nullptr)
	{
		// spAudio has different methods to access data like sampleRate, sampleCount and the audio data itself
		LogInfo("VoiceInput: Samples received: ", spAudio->getSampleCount());

		LogInfo("VoiceInput: Start processing");

		// 1. Store Audio locally as .wav (for example in system' TEMP folder)
		//    See this link for "how to save as wave": http://www.cplusplus.com/forum/beginner/166954/


		// Conversion of pointer types
		std::weak_ptr<const std::vector<short> > weakPointer = spAudio->getBuffer(); // weak pointer
		std::shared_ptr<const std::vector<short> > sharedPointer = weakPointer.lock(); // shared pointer
		std::vector<short> const * rawPointer = sharedPointer.get(); // raw pointer
		int count = spAudio->getSampleCount();	// short is 2 bytes  Size of vector = count of shorts in vector
		char *fileName = "new.wav";
		FILE *file = wavfile_open(fileName, spAudio->getSampleRate(), spAudio->getChannelCount());
		wavfile_write(file, rawPointer->data(), count);
		wavfile_close(file);

		// 2. Upload wave using CURL

		std::ifstream ifs(fileName, std::ios::binary | std::ios::ate);
		std::ifstream::pos_type pos = ifs.tellg();

		std::vector<char>  resultVoice(pos);
		ifs.seekg(0, ifs.end);
		int length = ifs.tellg();
		ifs.seekg(0, ifs.beg);
		ifs.read(&resultVoice[0], pos);
		std::string encodedAudio = base64_encode((unsigned char*)resultVoice.data(), length);


		CURL *curl;         // curl handle
		curl = curl_easy_init();
		CURLcode res;
		std::string postBuffer;
		std::string answerHeaderBuffer;
		std::string answerBodyBuffer;
		struct curl_slist* headers = NULL; // init to NULL is important
		headers = curl_slist_append(headers, "Content-Type: application/json"); // type is JSON
																				// Set options
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers); // apply header
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); // follow potential redirection
		curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, WriteCallbackVoice); // set callback for answer header
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallbackVoice); // set callback for answer body
		curl_easy_setopt(curl, CURLOPT_HEADERDATA, &answerHeaderBuffer); // set buffer for answer header
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &answerBodyBuffer); // set buffer for answer body
																	  // Post field
		const json jsonPost =
		{
			{ "config",{
				{ "encoding","LINEAR16" },
				{ "sampleRateHertz" , 44100 },
				{"maxAlternatives", 1},
				{ "languageCode" , "en-US" }
			} },
			{ "audio",{ { "content",encodedAudio } } }
		};

		postBuffer = jsonPost.dump(); // store in a string, otherwise it breaks
		curl_easy_setopt(curl, CURLOPT_URL, "https://speech.googleapis.com/v1/speech:recognize?key=AIzaSyCzq7vA9R1TZmQVOih9VFduv7Uok3j-Eaw");
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postBuffer);

		// Execute request
		res = curl_easy_perform(curl);
		if (res != CURLE_OK) // something went wrong
			LogError("Wrong request", curl_easy_strerror(res));

		// Cleanup
		curl_slist_free_all(headers); headers = nullptr;
		curl_easy_cleanup(curl); curl = nullptr;

		// Parse answer to JSON object and extract id token
		auto jsonAnswer = json::parse(answerBodyBuffer);
		
<<<<<<< HEAD
		//const std::string test = "{\"results\": [{\"alternatives\": [{\"transcript\": \"checkbox\",\"confidence\" : 0.80277747}]}]}";
=======
		//const std::string test = "{\"results\": [{\"alternatives\": [{\"transcript\": \"click ox\",\"confidence\" : 0.80277747}]}]}";
>>>>>>> 2c468da9488a36963d5edcc8dcb881eb331bf19e
		//auto jsonAnswer = json::parse(test);
		if (!jsonAnswer.empty()) {
			auto result = jsonAnswer["results"][0]["alternatives"][0];
			if (result.empty())
				LogError("VoiceInput: no result return");
			else {
				auto confidence = result["confidence"].get<float>();
				auto transcript = result["transcript"].get<std::string>();
				std::transform(transcript.begin(), transcript.end(), transcript.begin(), ::tolower);
				LogInfo("VoiceInput: ----------get voice script: ", transcript);
				// 4. Parse answer and return action

				std::string voiceCommand = "";
				int voicePara = 0;
				std::vector<std::string> keySplitList;
				std::vector<std::string> tranSplitList = split(transcript, ' ');
				int tranSplitListLen = tranSplitList.size();
				LogInfo("tran len ", tranSplitListLen);
				if (!transcript.empty()) {
					for (std::string key : voiceActionKeys) {
						//lev distance of key with transciption
						int levCom = 0;
						keySplitList = split(key, ' ');
						int keySplitListLen = keySplitList.size();
						//LogInfo("key ", key, " len: ", keySplitListLen);
						for (int i = 0; i < keySplitListLen; i++) {
							if (i < tranSplitListLen) {
								levCom += uiLevenshteinDistance(tranSplitList[i], keySplitList[i]);
								if (tranSplitList[i] == keySplitList[i] || levCom == 0 || (levCom < 2 && levCom != key.size()))
								{
									voiceCommand = key;
									voicePara = i + 1;
									LogInfo("VoiceInput: voice distance between transcript ( ", tranSplitList[i], " ) and ( ", keySplitList[i], " ) is ", levCom);
								}
							}
						}
						// min leven disctance						
					}
				}
				if (!voiceCommand.empty())
				{
					voiceResult.action = voiceActionMapping[voiceCommand];
					voiceResult.keyworkds = findPrefixAndParameters(voiceResult.action, tranSplitList, voicePara);
					return voiceResult;
				}
				else LogInfo("VoiceInput: sorry there is no such command: ", transcript);

			}
		}


	}


	else
	{
		LogInfo("VoiceInput: Failure");
	}

	// Fallback if error occurs
	return voiceResult;
}


/*
const char* _pURLregexExpression =
"(https?://)?"		// optional http or https
"([\\da-z\\.-]+)"	// domain name (any number, dot and character from a to z)
"\\."				// dot between name and domain
"([a-z\\.]{2,6})"	// domain itself
"([/\\w\\.:-]*)*"	// folder structure
"/?";				// optional last dash
std::unique_ptr<std::regex> _upIPregex;
const char* _pIPregexExpression =
"(https?://)?"		// optional http or https
"(\\d{1,3}(\\.\\d{1,3}){3})" // ip address
"([/\\w\\.:-]*)*"	// folder structure
"/?";				// optional last dash
std::unique_ptr<std::regex> _upFILEregex;
const char* _pFILEregexExpression =
"file:///"			// file prefix
".*";				// followed by anything
// Regular expression for URL validation
std::unique_ptr<std::regex> _upURLregex = std::make_unique<std::regex>(
	_pURLregexExpression,
	std::regex_constants::icase);
std::unique_ptr<std::regex> _upIPregex = std::make_unique<std::regex>(
	_pIPregexExpression,
	std::regex_constants::icase);
std::unique_ptr<std::regex> _upFILEregex = std::make_unique<std::regex>(
	_pFILEregexExpression,
	std::regex_constants::icase);
*/

/*std::string VoiceInput::EndAndProcessAudioRecordingWithDictationMode()
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


		// Conversion of pointer types
		std::weak_ptr<const std::vector<short> > weakPointer = spAudio->getBuffer(); // weak pointer
		std::shared_ptr<const std::vector<short> > sharedPointer = weakPointer.lock(); // shared pointer
		std::vector<short> const * rawPointer = sharedPointer.get(); // raw pointer
		int count = spAudio->getSampleCount();	// short is 2 bytes  Size of vector = count of shorts in vector
		char *fileName = "new.wav";
		FILE *file = wavfile_open(fileName, spAudio->getSampleRate(), spAudio->getChannelCount());
		wavfile_write(file, rawPointer->data(), count);
		wavfile_close(file);

		// 2. Upload wave using CURL

		std::ifstream ifs(fileName, std::ios::binary | std::ios::ate);
		std::ifstream::pos_type pos = ifs.tellg();

		std::vector<char>  resultVoice(pos);
		ifs.seekg(0, ifs.end);
		int length = ifs.tellg();
		ifs.seekg(0, ifs.beg);
		ifs.read(&resultVoice[0], pos);
		std::string encodedAudio = base64_encode((unsigned char*)resultVoice.data(), length);


		CURL *curl;         // curl handle
		curl = curl_easy_init();
		CURLcode res;
		std::string postBuffer;
		std::string answerHeaderBuffer;
		std::string answerBodyBuffer;
		struct curl_slist* headers = NULL; // init to NULL is important
		headers = curl_slist_append(headers, "Content-Type: application/json"); // type is JSON
																				// Set options
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers); // apply header
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); // follow potential redirection
		curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, WriteCallbackVoice); // set callback for answer header
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallbackVoice); // set callback for answer body
		curl_easy_setopt(curl, CURLOPT_HEADERDATA, &answerHeaderBuffer); // set buffer for answer header
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &answerBodyBuffer); // set buffer for answer body
																	  // Post field
		const json jsonPost =
		{
			{ "config",{
				{ "encoding","LINEAR16" },
				{ "sampleRateHertz" , 44100 },
				{ "languageCode" , "en-US" }
			} },
			{ "audio",{ { "content",encodedAudio } } }
		};

		postBuffer = jsonPost.dump(); // store in a string, otherwise it breaks
		curl_easy_setopt(curl, CURLOPT_URL, "https://speech.googleapis.com/v1/speech:recognize?key=AIzaSyCzq7vA9R1TZmQVOih9VFduv7Uok3j-Eaw");
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postBuffer);

		// Execute request
		res = curl_easy_perform(curl);
		if (res != CURLE_OK) // something went wrong
			LogError("Wrong request", curl_easy_strerror(res));

		// Cleanup
		curl_slist_free_all(headers); headers = nullptr;
		curl_easy_cleanup(curl); curl = nullptr;

		// Parse answer to JSON object and extract id token
		auto jsonAnswer = json::parse(answerBodyBuffer);
		if (!jsonAnswer.empty()) {
			auto result = jsonAnswer["results"][0]["alternatives"][0];
			if (result.empty())
				LogError("VoiceInput: no result return");
			else {
				auto confidence = result["confidence"].get<float>();
				auto transcript = result["transcript"].get<std::string>();
				//LogInfo("VoiceInput: voice script: ", transcript);
				// 4. Parse answer and return action
				int levDis = transcript.length();
				std::string voiceCommand = "";
				if (!transcript.empty()) {
				if(transcript.compare(0,5,"go to")==0)
				{
				voiceCommand = "go to";
				LogInfo("VoiceInput: prefix is ", "go to");

				}
				else if (transcript.compare(0, 10, "text input")==0)
				{
				voiceCommand = "text input";
				LogInfo("VoiceInput: prefix is ","text input" );

				}
				else if (transcript.compare(0, 11, "video input")==0)
				{
				voiceCommand = "video input";
				LogInfo("VoiceInput: prefix is ", "video input");

				}
				else if (transcript.compare(0, 6, "search")==0)
				{
				voiceCommand = "search";
				LogInfo("VoiceInput: prefix is ", "search");

				}
				else for (std::string key : voiceActionKeys) {
				int levCom = uiLevenshteinDistance(transcript.substr(0, maxLength - 1), key);
				if (levDis > levCom && levCom != key.length() && levCom <3)
				{
				levDis = levCom;
				voiceCommand = key;
				LogInfo("VoiceInput: voice distance between transcript ( ", transcript, " ) and ( ", key, " ) is ", levDis);
				}
				}
				}
				LogInfo("VoiceInput: " + transcript);
				return transcript;


			}
		}


	}
	else
	{
		LogInfo("VoiceInput: Failure");

	}

	// Fallback if error occurs
	//std::string result = u"";
	return "";
}

var key = {
'map'           : map,
'guide' : guide,
'show' : show,
'keep showing' : keepShowing,
'stop showing' : stopShowing,
'go to' : goTo,
'home' : home,
'down' : down,
'town' : down, // misheard word
'up' : up,
'op' : up,   // misheard word
'app' : up,   // misheard word
'right' : right,
'left' : left,
'fall' : fall,
'full' : fall, // misheard word
'song' : fall, // misheard word
'all' : fall, // misheard word
'rise' : rise,
'rice' : rise, // misheard word
'frys' : rise, // misheard word
'back' : back,
'forward' : forward,
'top' : top,
'bottom' : bottom,
'reload' : reload,
'refresh' : reload,
'zoom' : zoom,
'resume' : zoom, // misheard word
'zoomin' : zoom, // misheard word
'zoom in' : zoom,
'zoom out' : zoomOut,
'zoom normal' : zoomNormal,
'enhance' : enhance,
'help' : help,
'slower' : slower,
'flower' : slower, // misheard word
'faster' : faster,
'stop' : stop,
'hidehelp' : hideHelp,
'play' : play,
'pause' : pause,
'paws' : pause, // misheard word
'pawn' : pause, // misheard word
'mute' : muteVideo,
'unmute' : unmuteVideo,
'restart' : restart,
'skip' : skipForward,
'rewind' : skipBack,
'jump' : jumpForward,
'leap' : leapForward,
'increase volume' : increaseVolume,
'decrease volume' : decreaseVolume,
'blade runner mode' : toggleBRMode,
'keep scrolling down' : keepScrollingDown,
'keep scrolling up' : keepScrollingUp,
'keep scrolling right' : keepScrollingRight,
'keep scrolling left' : keepScrollingLeft
};
*/