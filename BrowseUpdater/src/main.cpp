#include <iostream>
#include <string>
#include <sstream>
#include <chrono>
#include <thread>
#include "externals/curl/include/curl/curl.h"

// Constants
const std::string serverURL = "https://userpages.uni-koblenz.de/~raphaelmenges/gtw-update";
const long long exitSleepMS = 1000;

// Variables
CURL *curl; // CURL handle
std::string readBuffer; // buffer
std::string downloadLink = "";

// Callback for CURL
static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

// Main
int main()
{
	// Welcome user
	std::cout << "### Updater of GazeTheWeb-Browse ###" << std::endl;

	// ### CHECK VERSION ###

	// TODO: check version of local gazetheweb
	
	// Initialize CURL
	curl = curl_easy_init();

	// Perform request of zip to to download
	if (curl)
	{
		// Setup CURL
		curl_easy_setopt(curl, CURLOPT_URL, std::string(serverURL + "/gtw-check.cgi?version=0.7").c_str()); // set address of request
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); // follow potential redirection
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback); // use write callback
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer); // set pointer to write data into

		// Perform the request
		CURLcode res = curl_easy_perform(curl);
		
		// Check for errors
		if (res != CURLE_OK)
		{
			std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res);
		}

		// Extract URL to zip to download
		std::stringstream ss(readBuffer);
		std::string token;
		int i = 0;
		while (std::getline(ss, token, '\n'))
		{
			if (i == 1 && !token.empty()) // second line and only when not empty
			{
				downloadLink = serverURL + "/" + token;
				break;
			}
			++i;
		}

		// Clean up CURL
		curl_easy_cleanup(curl);
	}

	// Only continue if download link it not empty
	if (downloadLink.empty())
	{
		std::cout << "No new version available for download. Exiting..." << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(exitSleepMS));
		return 0;
	}
	else
	{
		std::cout << "Downloading new version: " << downloadLink << "..." << std::endl;
	}

	// ### DOWNLOAD NEW VERSION INTO TMP ###

	// ### UNPACK NEW VERSION INTO TMP ##

	// ### REMOVE OLD VERSION ###

	// ### COPY NEW VERSION ###

	// ### RETURN ###

	// Require user input to force console to stay open
	char input;
	// std::cin >> input;

	// Return from main
	std::this_thread::sleep_for(std::chrono::milliseconds(exitSleepMS));
	return 0;
}