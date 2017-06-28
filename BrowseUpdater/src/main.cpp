#include <iostream>
#include <string>
#include <sstream>
#include <chrono>
#include <thread>
#include "externals/curl/include/curl/curl.h"

// Constants
const std::string serverURL = "https://userpages.uni-koblenz.de/~raphaelmenges/gtw-update";
const char tmpZipPath[FILENAME_MAX] = "C:/Users/Raphael/Desktop/gtw_new_version.zip";
const long long exitSleepMS = 5000;

// Variables
CURL *curl; // CURL handle
std::string readBuffer; // buffer
std::string downloadLink = "";


// Callback for CURL to retrieve zip name
size_t WriteURL(void *contents, size_t size, size_t nmemb, void *userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

// Callback for CURL to retrieve zip file
size_t WriteData(void *ptr, size_t size, size_t nmemb, FILE *stream) {
	size_t written = fwrite(ptr, size, nmemb, stream);
	std::cout << ".";
	return written;
}

// End function
int Return(std::string message)
{
	std::cout << message << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(exitSleepMS));
	return 0;
}

// Main
int main()
{
	// Welcome user
	std::cout << "####################################" << std::endl;
	std::cout << "### Updater of GazeTheWeb-Browse ###" << std::endl;
	std::cout << "####################################" << std::endl;
	std::cout << "By Raphael Menges" << std::endl;
	std::cout << std::endl;

	// ### CHECK VERSION ###

	// TODO: check version of local gazetheweb
	
	// Initialize CURL
	curl = curl_easy_init();

	// Perform request of zip to to download
	if (curl)
	{
		// Setup CURL
		curl_easy_setopt(curl, CURLOPT_URL, std::string(serverURL + "/gtw-check.cgi?version=0.6").c_str()); // set address of request
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); // follow potential redirection
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteURL); // use write callback
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer); // set pointer to write data into

		// Perform the request
		CURLcode res = curl_easy_perform(curl);
		
		// Check for errors
		if (res != CURLE_OK)
		{
			curl_easy_cleanup(curl);
			return Return("curl_easy_perform() failed: " + std::string(curl_easy_strerror(res)) + " Exiting...");
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
		return Return("No new version available for download. Exiting...");
	}
	else
	{
		std::cout << "Downloading new version: " << downloadLink << "..." << std::endl;
	}

	// ### DOWNLOAD NEW VERSION INTO TMP ###
	curl = curl_easy_init();
	if (curl)
	{
		FILE *fp;
		fp = fopen(tmpZipPath, "wb");
		if (fp != NULL) // Only continue if file is opened
		{
			// 
			curl_easy_setopt(curl, CURLOPT_URL, downloadLink.c_str());
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteData);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
			CURLcode res = curl_easy_perform(curl);

			// Check for errors
			if (res != CURLE_OK)
			{
				curl_easy_cleanup(curl);
				return Return("curl_easy_perform() failed: " + std::string(curl_easy_strerror(res)) + " Exiting...");
			}

			// Cleanup CURL and close file
			curl_easy_cleanup(curl);
			fclose(fp);
		}
		else
		{
			return Return("Target path for tmp download could be opened. Exiting...");
		}
		std::cout << "...download done." << std::endl;
	}
	else
	{
		return Return("CURL could not be instantiated. Exiting...");
	}

	// ### UNPACK NEW VERSION INTO TMP ##

	// Unpack zip

	// Remove zip
	// std::cout << "Removing tmp zip file..." << std::endl;
	// std::remove(tmpZipPath);

	// ### REMOVE OLD VERSION ###

	// ### COPY NEW VERSION ###

	// ### RETURN ###

	// Return from main
	return Return("Success! Exiting updater...");
}