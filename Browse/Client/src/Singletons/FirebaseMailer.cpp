//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "FirebaseMailer.h"
#include "src/Utils/glmWrapper.h"
#include "src/Utils/Logger.h"
#include "externals/curl/include/curl/curl.h"
#include <sstream>
#include <chrono>

// ### TEMPLATE HELPERS ###

// Fallback for values when not found in database etc.
template<typename Type> Type fallback();
template<> int fallback<int>() { return 0; };
template<> std::string fallback<std::string>() { return ""; };
template<> json fallback<json>() { return json(); };

// ### HTTP HELPERS ###
size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

bool HttpHeaderFirstLineContains(const std::string& rHeader, const std::string& rCompareTo)
{
	std::string firstLine = rHeader.substr(0, rHeader.find("\n"));
	return firstLine.find(rCompareTo) != std::string::npos;
}

bool HttpHeaderOK(const std::string& rHeader)
{
	return HttpHeaderFirstLineContains(rHeader, "200 OK");
}

bool HttpHeaderPreconditionFailed(const std::string& rHeader)
{
	return HttpHeaderFirstLineContains(rHeader, "412 Precondition Failed");
}

std::string HttpHeaderExtractETag(const std::string& rHeader)
{
	std::istringstream lineStream(rHeader);
	std::string line;
	std::getline(lineStream, line); // first line is not of interest
	while(std::getline(lineStream, line)) // go over lines
	{
		std::istringstream tokenStream(line);
		std::string token;
		std::vector<std::string> tokens;
		while (std::getline(tokenStream, token, ':')) // go over tokens
		{
			tokens.push_back(token); // collect tokens
		}

		// Should be two tokens, and first should be "ETag"
		if (tokens.size() == 2 && tokens.at(0) == "ETag")
		{
			// Extract second token as ETag
			return tokens.at(1).substr(1, tokens.at(1).length() - 1); // return without preceeding space
		}
	}

	return ""; // return empty string in case of non exisiting ETag
}
// ###############

bool FirebaseMailer::FirebaseInterface::Login(std::string email, std::string password)
{
	// Invalidate tokens
	_idToken = "";
	_refreshToken = "";

	// Setup CURL
	CURL *curl;
	curl = curl_easy_init();

	// Continue if CURL was initialized
	if (curl)
	{
		// Local variables
		CURLcode res;
		std::string postBuffer;
		std::string answerHeaderBuffer;
		std::string answerBodyBuffer;
		struct curl_slist* headers = NULL; // init to NULL is important 
		headers = curl_slist_append(headers, "Content-Type: application/json"); // type is JSON
		
		// Set options
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers); // apply header
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); // follow potential redirection
		curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, WriteCallback); // set callback for answer header
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback); // set callback for answer body
		curl_easy_setopt(curl, CURLOPT_HEADERDATA, &answerHeaderBuffer); // set buffer for answer header
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &answerBodyBuffer); // set buffer for answer body

		// Post field
		const json jsonPost =
		{
			{ "email", email }, // email of user
			{ "password", password }, // password to access database
			{ "returnSecureToken", true } // of course, thats what this is about
		};
		postBuffer = jsonPost.dump(); // store in a string, otherwise it breaks

		// Fill request
		curl_easy_setopt(curl, CURLOPT_URL, "https://www.googleapis.com/identitytoolkit/v3/relyingparty/verifyPassword?key=" + _API_KEY); // URL to access
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postBuffer); // fill post body
		
		// Execute request
		res = curl_easy_perform(curl);
		if (res != CURLE_OK) // something went wrong
		{
			LogError("FirebaseInterface: ", "User login to Firebase failed: ", curl_easy_strerror(res));
			return false;
		}

		// Cleanup
		curl_slist_free_all(headers); headers = nullptr;
		curl_easy_cleanup(curl); curl = nullptr;

		// Parse answer to JSON object and extract id token
		auto jsonAnswer = json::parse(answerBodyBuffer);
		auto result = jsonAnswer.find("idToken");
		if (result != jsonAnswer.end())
		{
			_idToken = result.value().get<std::string>();
			LogInfo("FirebaseInterface: ", "User successfully logged into Firebase.");
		}
		else
		{

			LogError("FirebaseInterface: ", "User login to Firebase failed.");
		}

		// Search for refresh token (optional)
		result = jsonAnswer.find("refreshToken");
		if (result != jsonAnswer.end())
		{
			_refreshToken = result.value().get<std::string>();
		}

		// Search for uid
		result = jsonAnswer.find("localId");
		if (result != jsonAnswer.end())
		{
			_uid = result.value().get<std::string>(); // one could "break mailer" if not provided as nothing makes sense
		}
	}
	return !_idToken.empty(); // id token is not empty, so something happened
}

std::pair<std::string, json> FirebaseMailer::FirebaseInterface::Get(std::string key)
{
	// Return value
	std::pair<std::string, json> result;

	// Only continue if logged in
	if (!_idToken.empty())
	{
		// Setup CURL
		CURL *curl;
		curl = curl_easy_init();

		// Continue if CURL was initialized
		if (curl)
		{
			// Local variables
			std::string answerHeaderBuffer;
			std::string answerBodyBuffer;
			struct curl_slist* headers = NULL; // init to NULL is important 
			headers = curl_slist_append(headers, "X-Firebase-ETag: true"); // tell it to deliver ETag to identify the state

			// Request URL
			std::string requestURL =
				_URL + "/"
				+ key
				+ ".json"
				+ "?auth=" + _idToken;

			// Setup CURL
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers); // apply header
			curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); // follow potential redirection
			curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, WriteCallback); // set callback for answer header
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback); // set callback for answer body
			curl_easy_setopt(curl, CURLOPT_HEADERDATA, &answerHeaderBuffer); // set buffer for answer header
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &answerBodyBuffer); // set buffer for answer body
			curl_easy_setopt(curl, CURLOPT_URL, requestURL.c_str()); // set address of request

			// Perform the request
			CURLcode res = curl_easy_perform(curl);
			if (res == CURLE_OK) // everything ok with CURL
			{
				// Check header for OK
				if (HttpHeaderOK(answerHeaderBuffer))
				{
					// Parse
					result = std::make_pair(HttpHeaderExtractETag(answerHeaderBuffer), json::parse(answerBodyBuffer));
				}
				else // not ok, guess timeout of id token?
				{
					// Try to relogin
					if (Relogin() && !_idToken.empty()) // returns whether successful
					{
						// Setup CURL request (reuse stuff from first)
						answerHeaderBuffer.clear();
						answerBodyBuffer.clear();
						requestURL =
							_URL + "/"
							+ key
							+ ".json"
							+ "?auth=" + _idToken; // update request URL as id token should be new
						curl_easy_setopt(curl, CURLOPT_URL, requestURL.c_str()); // set address of new request

						 // Try again to GET data
						res = curl_easy_perform(curl);
						if (res == CURLE_OK && HttpHeaderOK(answerHeaderBuffer)) // everything ok with CURL and header
						{
							// Parse
							result = std::make_pair(HttpHeaderExtractETag(answerHeaderBuffer), json::parse(answerBodyBuffer));
						}
					}
				}
			}

			// Cleanup of CURL
			curl_easy_cleanup(curl); curl = nullptr;
		}
	}

	// Return result
	return result;
}

template<typename T>
void FirebaseMailer::FirebaseInterface::Put(T key, typename FirebaseValue<T>::type value)
{
	// Call own get method to get the value if already set
	std::pair<std::string, json> DBvalue = this->Get(BuildFirebaseKey(key, _uid)); // here we are only interesed in the ETag

	// Put value in database
	std::string ETag = DBvalue.first;
	std::string newETag = "";
	typename FirebaseValue<T>::type newValue = fallback<typename FirebaseValue<T>::type>();
	bool success = false;
	const int maxTrialCount = 10;
	int trialCount = 0;

	// Try as long as no success but still new ETags
	do
	{
		// Try to put on database
		newETag = "";
		newValue = fallback<typename FirebaseValue<T>::type>();
		success = Put(key, ETag, value, newETag, newValue);

		// If empty new ETag, just quit this (either success or database just does not like us)
		if (newETag.empty()) { break; }

		// Store new values
		ETag = newETag;
		// value = newValue; -> this would override the value to put

		// Increase trial count
		++trialCount;

	} while (!success && trialCount <= maxTrialCount);

	// Tell user about no success
	if (!success)
	{
		LogError("FirebaseInterface: ", "Data transfer to Firebase failed.");
	}
}

void FirebaseMailer::FirebaseInterface::Transform(FirebaseIntegerKey key, int delta)
{
	// Just add the delta to the existing value
	Apply(key, [delta](int DBvalue) { return DBvalue + delta; });
}

void FirebaseMailer::FirebaseInterface::Maximum(FirebaseIntegerKey key, int value)
{
	// Use maximum of database value and this
	Apply(key, [value](int DBvalue) { return glm::max(DBvalue, value); });
}

bool FirebaseMailer::FirebaseInterface::Relogin()
{
	// Store refresh token locally
	std::string refreshToken = _refreshToken;

	// Invalidate tokens
	_idToken = "";
	_refreshToken = "";

	// Check for nonempty refresh token
	if (!refreshToken.empty())
	{
		// Setup CURL
		CURL *curl;
		curl = curl_easy_init();

		// Continue if CURL was initialized
		if (curl)
		{
			// Local variables
			CURLcode res;
			std::string answerBodyBuffer;
			struct curl_slist* headers = NULL; // init to NULL is important 
			headers = curl_slist_append(headers, "Content-Type: application/x-www-form-urlencoded"); // type is simple form encoding

			// Set options
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers); // apply header
			curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); // follow potential redirection
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback); // set callback for answer body
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &answerBodyBuffer); // set buffer for answer body

			// Post field
			const std::string postBuffer = "grant_type=refresh_token&refresh_token=" + refreshToken;

			// Fill request
			curl_easy_setopt(curl, CURLOPT_URL, "https://securetoken.googleapis.com/v1/token?key=" + _API_KEY); // URL to access
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postBuffer); // fill post body

			// Execute request
			res = curl_easy_perform(curl);
			if (res != CURLE_OK) // something went wrong
			{
				LogError("FirebaseInterface: ", "User reauthentifiation to Firebase failed: ", curl_easy_strerror(res));
				return false;
			}

			// Cleanup
			curl_slist_free_all(headers); headers = nullptr;
			curl_easy_cleanup(curl); curl = nullptr;

			// Parse answer to JSON object and extract id token
			auto jsonAnswer = json::parse(answerBodyBuffer);
			auto result = jsonAnswer.find("id_token"); // different from email and password login
			if (result != jsonAnswer.end())
			{
				_idToken = result.value().get<std::string>();
				LogInfo("FirebaseInterface: ", "User reauthentifiation to Firebase successful.");
			}
			else
			{
				LogError("FirebaseInterface: ", "User reauthentifiation to Firebase failed.");
			}

			// Search for refresh token (optional)
			result = jsonAnswer.find("refresh_token");
			if (result != jsonAnswer.end())
			{
				_refreshToken = result.value().get<std::string>();
			}
		}
	}

	return !_idToken.empty(); // ok if id token has been filled
}

template<typename T>
bool FirebaseMailer::FirebaseInterface::Put(T key, std::string ETag, typename FirebaseValue<T>::type value, std::string& rNewETag, typename FirebaseValue<T>::type& rNewValue)
{
	bool success = false;
	rNewETag = "";
	rNewValue = fallback<typename FirebaseValue<T>::type>();

	// Only continue if logged in
	if (!_idToken.empty())
	{
		// Setup CURL
		CURL *curl;
		curl = curl_easy_init();

		// Continue if CURL was initialized
		if (curl)
		{
			// Local variables
			std::string answerHeaderBuffer;
			std::string answerBodyBuffer;
			struct curl_slist* headers = NULL; // init to NULL is important
			const std::string putHeaderBuffer = "if-match:" + ETag;
			headers = curl_slist_append(headers, putHeaderBuffer.c_str()); // 'if-match' criteria to use the ETag

			// Request URL
			std::string requestURL =
				_URL + "/"
				+ BuildFirebaseKey(key, _uid)
				+ ".json"
				+ "?auth=" + _idToken;

			// Setup CURL
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers); // apply header
			curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); // follow potential redirection
			curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT"); // tell about PUTting
			curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L); // CURL told me to use it
			curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, WriteCallback); // set callback for answer header
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback); // set callback for answer body
			curl_easy_setopt(curl, CURLOPT_HEADERDATA, &answerHeaderBuffer); // set buffer for answer header
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &answerBodyBuffer); // set buffer for answer body

			// Post field
			const std::string postField = json(value).dump();

			// Fill request
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postField.c_str()); // value to put
			curl_easy_setopt(curl, CURLOPT_URL, requestURL.c_str()); // set address of request

			// Perform the request
			CURLcode res = curl_easy_perform(curl);
			if (res == CURLE_OK) // everything ok with CURL
			{
				// Check header
				if (HttpHeaderOK(answerHeaderBuffer))
				{
					success = true; // fine!
				}
				else if (HttpHeaderPreconditionFailed(answerHeaderBuffer))
				{
					// Fill newETag and newValue
					rNewETag = HttpHeaderExtractETag(answerHeaderBuffer);
					auto DBvalue = json::parse(answerBodyBuffer);
					// if (DBvalue.is_number_integer())
					{
						rNewValue = DBvalue.get<typename FirebaseValue<T>::type>();
					}
				}
				else // there is something else in header
				{
					// Try to relogin
					if (Relogin() && !_idToken.empty()) // returns whether successful
					{
						// Setup CURL request (reuse stuff from first)
						answerHeaderBuffer.clear();
						answerBodyBuffer.clear();
						requestURL =
							_URL + "/"
							+ BuildFirebaseKey(key, _uid)
							+ ".json"
							+ "?auth=" + _idToken;
						curl_easy_setopt(curl, CURLOPT_URL, requestURL.c_str()); // set address of new request

						// Try again to PUT data
						res = curl_easy_perform(curl);
						if (res == CURLE_OK) // everything ok with CURL
						{
							if (HttpHeaderOK(answerHeaderBuffer))
							{
								success = true; // fine!
							}
							else if (HttpHeaderPreconditionFailed(answerHeaderBuffer))
							{
								// Fill newETag and newValue
								rNewETag = HttpHeaderExtractETag(answerHeaderBuffer);
								auto DBvalue = json::parse(answerBodyBuffer);
								// if (DBvalue.is_number_integer())
								{
									rNewValue = DBvalue.get<typename FirebaseValue<T>::type>();
								}
							}
							// else: ok, it failed.
						}
					}
				}
			}

			// Cleanup
			curl_slist_free_all(headers); headers = nullptr;
			curl_easy_cleanup(curl); curl = nullptr;
		}
	}

	return success;
}

void FirebaseMailer::FirebaseInterface::Apply(FirebaseIntegerKey key, std::function<int(int)> function)
{
	// Call own get method
	std::pair<std::string, json> DBvalue = this->Get(BuildFirebaseKey(key, _uid));

	// Check whether data was found, create new with single zero integer value, if necessary
	int value = (!DBvalue.second.empty() && DBvalue.second.is_number_integer()) ? DBvalue.second.get<int>() : 0;

	// Apply function on the value
	value = function(value);

	// Put value in database
	std::string ETag = DBvalue.first;
	std::string newETag = "";
	int newValue = 0;
	bool success = false;
	const int maxTrialCount = 10;
	int trialCount = 0;

	// Try as long as no success but still new ETags
	do
	{
		// Try to put on database
		newETag = "";
		newValue = 0;
		success = Put(key, ETag, value, newETag, newValue);

		// If empty new ETag, just quit this (either success or database just does not like us)
		if (newETag.empty()) { break; }

		// Store new values
		ETag = newETag;
		value = newValue;

		// Transform retrieved value
		value = function(value);

		// Increase trial count
		++trialCount;

	} while (!success && trialCount <= maxTrialCount);

	// Tell user about no success
	if (!success)
	{
		LogError("FirebaseInterface: ", "Data transfer to Firebase failed.");
	}
}

FirebaseMailer::FirebaseMailer()
{
	// Create thread where FirebaseInterface lives in
	auto* pMutex = &_mutex;
	auto* pConditionVariable = &_conditionVariable;
	auto* pCommandQueue = &_commandQueue;
	auto const * pShouldStop = &_shouldStop;
	_upThread = std::unique_ptr<std::thread>(new std::thread([pMutex, pConditionVariable, pCommandQueue, pShouldStop]() // pass copies of pointers to members
	{
		// Create interface to firebase
		FirebaseInterface interface;

		// Local command queue
		std::deque<std::shared_ptr<Command> > localCommandQueue;

		// While loop to work on commands
		while (!*pShouldStop) // exits when should stop, so thread can be joined
		{
			// Wait for data
			{
				std::unique_lock<std::mutex> lock(*pMutex); // acquire lock
				pConditionVariable->wait(
					lock, // lock that is handled by condition variable
					[pCommandQueue, pShouldStop]
					{
						return !pCommandQueue->empty() || *pShouldStop; // condition is either there are some commands or it is called to stop and become joinable
					}); // hand over locking to the conidition variable which waits for notification by main thread
				localCommandQueue = std::move(*pCommandQueue); // move content of command queue to local one
				pCommandQueue->clear(); // clear original queue
				lock.unlock(); // do not trust the scope stuff. But should be not necessary
			}

			// Work on commands
			for (const auto& rCommand : localCommandQueue)
			{
				(*rCommand.get())(interface);
			}
		}
	}));
}

void FirebaseMailer::PushBack_Login(std::string email, std::string password)
{
	// Add command to queue, take parameters as copy
	PushBackCommand(std::shared_ptr<Command>(new Command([=](FirebaseInterface& rInterface)
	{
		rInterface.Login(email, password);
	})));
}

void FirebaseMailer::PushBack_Transform(FirebaseIntegerKey key, int delta)
{
	// Add command to queue, take parameters as copy
	PushBackCommand(std::shared_ptr<Command>(new Command([=](FirebaseInterface& rInterface)
	{
		rInterface.Transform(key, delta);
	})));
}

void FirebaseMailer::PushBack_Maximum(FirebaseIntegerKey key, int value)
{
	// Add command to queue, take parameters as copy
	PushBackCommand(std::shared_ptr<Command>(new Command([=](FirebaseInterface& rInterface)
	{
		rInterface.Maximum(key, value);
	})));
}

void FirebaseMailer::PushBack_Put(FirebaseIntegerKey key, int value)
{
	// Add command to queue, take parameters as copy
	PushBackCommand(std::shared_ptr<Command>(new Command([=](FirebaseInterface& rInterface)
	{
		rInterface.Put(key, value);
	})));
}

void FirebaseMailer::PushBack_Put(FirebaseStringKey key, std::string value)
{
	// Add command to queue, take parameters as copy
	PushBackCommand(std::shared_ptr<Command>(new Command([=](FirebaseInterface& rInterface)
	{
		rInterface.Put(key, value);
	})));
}

void FirebaseMailer::PushBack_Put(FirebaseJSONKey key, json value)
{
	// Add command to queue, take parameters as copy
	PushBackCommand(std::shared_ptr<Command>(new Command([=](FirebaseInterface& rInterface)
	{
		rInterface.Put(key, value);
	})));
}

void FirebaseMailer::PushBackCommand(std::shared_ptr<Command> spCommand)
{
	if (!_paused) // only push back the command if not paused
	{
		std::lock_guard<std::mutex> lock(_mutex);
		_commandQueue.push_back(spCommand); // push back command to queue
		_conditionVariable.notify_all(); // notify thread about new data
	}
}