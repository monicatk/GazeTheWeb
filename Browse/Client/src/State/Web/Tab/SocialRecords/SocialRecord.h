//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Structure to hold a social record like a Facebook visit.

#ifndef SOCIALRECORD_H_
#define SOCIALRECORD_H_

#include "src/Utils/Helper.h"
#include "submodules/json/src/json.hpp"
#include <string>
#include <chrono>

// List of social platforms (for each a specialization of this exists)
enum class SocialPlatform
{
	Facebook, Linkedin, YouTube, Unknown
};

// Class of social record
class SocialRecord
{
public:

	// Function to classify current URL
	static SocialPlatform ClassifyURL(std::string URL)
	{
		std::string shortURL = ShortenURL(URL);

		std::string searchString = "youtube";

		// Check for platform
		size_t found = shortURL.find(searchString);
		SocialPlatform platform = SocialPlatform::Unknown;
		if (found != std::string::npos)
		{
			platform = SocialPlatform::YouTube;
		}

		return platform;
	}

	// Constructor
	SocialRecord(std::string URL);

	// Virtual destructor
	virtual ~SocialRecord();

	// Add time in foreground
	void AddTimeInForeground(float time);

	// Add time in foreground and user is active
	void AddTimeActiveUser(float time);

	// Add scrolling delta
	void AddScrollingDelta(float delta);

	// Add click
	void AddClick(); // TODO call

	// Add subpage
	void AddSubpage();

	// End record
	void End();

	// Convert to JSON for storing in database
	nlohmann::json ToJSON() const;

	// Get platform
	SocialPlatform GetPlatform() const { return _platform; }

protected:

	// Constructor with ability to set different platform
	SocialRecord(std::string URL, SocialPlatform platform);

	// Get date
	std::string GetDate() const;

private:

	// Members
	const std::string DATE_FORMAT = "%d-%m-%Y %H-%M-%S";
	const SocialPlatform _platform = SocialPlatform::YouTube; // TODO change back to unkown
	const std::string _URL; // URL where this was started this
	std::chrono::time_point<std::chrono::system_clock> _startTime;
	std::chrono::time_point<std::chrono::system_clock> _endTime;
	std::string _startDate;
	std::string _endDate;
	double _durationInForeground = 0.0;
	double _durationUserActive = 0.0; // and tab in foreground
	double _scrollAmount = 0.0;
	int _subpageCount = 0;
	int _clickCount = 0;
};

#endif // SOCIALRECORD_H_