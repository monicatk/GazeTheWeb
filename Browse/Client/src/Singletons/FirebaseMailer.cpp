//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "FirebaseMailer.h"
#include "src/Utils/glmWrapper.h"
#include "src/Utils/Logger.h"
#include "externals/curl/include/curl/curl.h"
#include <sstream>

// ### Helpers ###
size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

bool HttpHeaderFirstLineFind(const std::string& rHeader, const std::string& rCompareTo)
{
	std::string firstLine = rHeader.substr(0, rHeader.find("\n"));
	return firstLine.find(rCompareTo) != std::string::npos;
}

bool HttpHeaderOK(const std::string& rHeader)
{
	return HttpHeaderFirstLineFind(rHeader, "200 OK");
}

bool HttpHeaderPreconditionFailed(const std::string& rHeader)
{
	return HttpHeaderFirstLineFind(rHeader, "412 Precondition Failed");
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

bool FirebaseMailer::Login(std::string email, std::string password)
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
		curl_easy_setopt(curl, CURLOPT_URL, "https://www.googleapis.com/identitytoolkit/v3/relyingparty/verifyPassword?key=" + API_KEY); // URL to access
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postBuffer); // fill post body
		
		// Execute request
		res = curl_easy_perform(curl);
		if (res != CURLE_OK) // something went wrong
		{
			LogError("FirebaseMailer: ", "User login to Firebase failed: ", curl_easy_strerror(res));
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
			LogInfo("FirebaseMailer: ", "User successfully logged into Firebase.");
		}
		else
		{

			LogError("FirebaseMailer: ", "User login to Firebase failed.");
		}

		// Search for refresh token (optional)
		result = jsonAnswer.find("refreshToken");
		if (result != jsonAnswer.end())
		{
			_refreshToken = result.value().get<std::string>();
		}
	}
	return !_idToken.empty(); // id token is not empty, so something happened
}

std::pair<std::string, json> FirebaseMailer::Get(FirebaseKey key)
{
	return Get(FirebaseKeyString.at(key));
}

std::pair<std::string, json> FirebaseMailer::Get(std::string key)
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
				URL + "/"
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
							URL + "/"
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

void FirebaseMailer::Transform(FirebaseKey key, int delta)
{
	// Just add the delta to the existing value
	Apply(key, [delta](int DBvalue) { return DBvalue + delta; });
}

void FirebaseMailer::Maximum(FirebaseKey key, int value)
{
	// Use maximum of database value and this
	Apply(key, [value](int DBvalue) { return glm::max(DBvalue, value); });
}

bool FirebaseMailer::Relogin()
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
			std::string postBuffer = "grant_type=refresh_token&refresh_token=" + refreshToken;

			// Fill request
			curl_easy_setopt(curl, CURLOPT_URL, "https://securetoken.googleapis.com/v1/token?key=" + API_KEY); // URL to access
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postBuffer); // fill post body

			// Execute request
			res = curl_easy_perform(curl);
			if (res != CURLE_OK) // something went wrong
			{
				LogError("FirebaseMailer: ", "User reauthentifiation to Firebase failed: ", curl_easy_strerror(res));
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
				LogInfo("FirebaseMailer: ", "User reauthentifiation to Firebase successful.");
			}
			else
			{
				LogError("FirebaseMailer: ", "User reauthentifiation to Firebase failed.");
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

void FirebaseMailer::Apply(FirebaseKey key, std::function<int(int)>)
{
	// TODO: implement concurrency proof database writing via ETag!
	// TODO: if value does not exist, create it with zero!


}