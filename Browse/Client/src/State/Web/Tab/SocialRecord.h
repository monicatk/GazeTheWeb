//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Structure to hold a social record, for example a Facebook visit.

#ifndef SOCIALRECORD_H_
#define SOCIALRECORD_H_

#include "src/Utils/Helper.h"
#include "src/Singletons/FirebaseMailer.h"
#include "submodules/json/src/json.hpp"
#include <string>
#include <chrono>

// List of social platforms
enum class SocialPlatform
{
	Unknown, Facebook, Linkedin, YouTube
};

// List of identifying domain parts for social platforms
static const std::map < SocialPlatform, std::vector<std::string> > SocialIdentifiers =
{
	{ SocialPlatform::Facebook,{ "facebook.com" } },
	{ SocialPlatform::Linkedin,{ "linkedin.com" } },
	{ SocialPlatform::YouTube,{ "youtube.com", "youtu.be" } }
};

// Mapping from platform to database keys (pair of count and record key)
static const std::map <SocialPlatform, std::pair<FirebaseIntegerKey, FirebaseJSONKey> > SocialFirebaseKeys =
{
	{ SocialPlatform::Unknown,	std::make_pair(FirebaseIntegerKey::SOCIAL_RECORD_UNKNOWN_COUNT,		FirebaseJSONKey::SOCIAL_RECORD_UNKNOWN) },
	{ SocialPlatform::Facebook,	std::make_pair(FirebaseIntegerKey::SOCIAL_RECORD_FACEBOOK_COUNT,	FirebaseJSONKey::SOCIAL_RECORD_FACEBOOK) },
	{ SocialPlatform::Linkedin,	std::make_pair(FirebaseIntegerKey::SOCIAL_RECORD_LINKEDIN_COUNT,	FirebaseJSONKey::SOCIAL_RECORD_LINKEDIN) },
	{ SocialPlatform::YouTube,	std::make_pair(FirebaseIntegerKey::SOCIAL_RECORD_YOUTUBE_COUNT,		FirebaseJSONKey::SOCIAL_RECORD_YOUTUBE) },
};

// Class of social record
class SocialRecord
{
public:

	// Static function to classify current URL. Does return unknown when social platform cannot be determined
	static SocialPlatform ClassifyURL(std::string URL)
	{
		// Extract domain or URL
		const std::string shortURL = ShortenURL(URL);

		// Go over social identifiers and search for them in the domain
		SocialPlatform platform = SocialPlatform::Unknown;
		for (const auto& rPlatform : SocialIdentifiers) // go over platforms
		{
			for (const auto& rIdentifier : rPlatform.second) // go over identifiers
			{
				if (shortURL.find(rIdentifier) != std::string::npos) // search for identifier
				{
					platform = rPlatform.first; // found the platform
					return platform; // instead of break
				}
			}
		}

		// Return result
		return platform;
	}

	// Constructor
	SocialRecord(std::string URL, SocialPlatform platform);

	// Virtual destructor
	virtual ~SocialRecord();

	// Start recrod
	void Start();

	// End record
	void End();

	// Persist record (sends it to Firebase)
	void Persist();

	// Add time in foreground
	void AddTimeInForeground(float time);

	// Add time in foreground and user is active
	void AddTimeActiveUser(float time);

	// Add scrolling delta
	void AddScrollingDelta(float delta);

	// Add click
	void AddClick(); // TODO call

	// Add subpage
	void AddSubpage(std::string URL);

	// Convert to JSON for storing in database
	nlohmann::json ToJSON() const;

	// Get platform
	SocialPlatform GetPlatform() const { return _platform; }

	// Get (initial) URL
	std::string GetURL() const { return _URL; }

	// Get domain
	std::string GetDomain() const { return ShortenURL(_URL); }

protected:

	// Get date
	std::string GetDate() const;

private:

	// Members
	const std::string DATE_FORMAT = "%d-%m-%Y %H-%M-%S";
	const SocialPlatform _platform = SocialPlatform::Unknown;
	const std::string _URL; // URL where this was started this
	bool _writeable = false;
	std::chrono::time_point<std::chrono::system_clock> _startTime;
	std::chrono::time_point<std::chrono::system_clock> _endTime;
	std::string _startDate;
	std::string _endDate;
	double _durationInForeground = 0.0;
	double _durationUserActive = 0.0; // and tab in foreground
	double _scrollAmount = 0.0;
	std::vector<std::string> _subpages;
	int _clickCount = 0;
};

#endif // SOCIALRECORD_H_