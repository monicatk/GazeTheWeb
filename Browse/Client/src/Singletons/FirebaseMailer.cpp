//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "FirebaseMailer.h"
#include "src/Utils/glmWrapper.h"
#include "src/Utils/Logger.h"
#include "externals/curl/include/curl/curl.h"

// ### Helpers ###
size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}
// ###############

bool FirebaseMailer::Login(std::string email, std::string password)
{
	// Reset member token
	_token = "";

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

		// Parse answer to JSON object and extract token
		auto jsonAnswer = json::parse(answerBodyBuffer);
		auto result = jsonAnswer.find("idToken");
		if (result != jsonAnswer.end())
		{
			_token = result.value().get<std::string>();
			LogInfo("FirebaseMailer: ", "User successfully logged into Firebase");
		}
		else
		{

			LogError("FirebaseMailer: ", "User login to Firebase failed.");
		}
	}
	return !_token.empty();
}

json FirebaseMailer::Get(FirebaseKey key)
{
	return Get(FirebaseKeyString.at(key));
}

json FirebaseMailer::Get(std::string key)
{
	// TODO check for token failure and try to relogin
	if (!_token.empty())
	{
		// Setup CURL
		CURL *curl;
		curl = curl_easy_init();

		// Continue if CURL was initialized
		if (curl)
		{
			// Local variables
			std::string answerBuffer;
			const std::string requestURL =
				URL + "/"
				+ key
				+ ".json"
				+ "?auth=" + _token;

			// Setup CURL
			curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); // follow potential redirection
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback); // use write callback
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &answerBuffer); // set pointer to write data into
			curl_easy_setopt(curl, CURLOPT_URL, requestURL.c_str()); // set address of request

			// Perform the request
			CURLcode res = curl_easy_perform(curl);
			if (res != CURLE_OK) // something went wrong
			{
				// TODO: Log?
				return json();
			}

			// Parse
			return json::parse(answerBuffer);
		}
	}
	return json(); // return empty json as fallback
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

void FirebaseMailer::Apply(FirebaseKey key, std::function<int(int)>)
{
	// TODO: implement concurrency proof database writing via ETag!
}