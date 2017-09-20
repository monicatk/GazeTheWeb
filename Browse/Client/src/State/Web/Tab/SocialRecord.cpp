//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "SocialRecord.h"
#include <iomanip>
#include <ctime>
#include <sstream>

using json = nlohmann::json;

SocialRecord::SocialRecord(std::string URL, SocialPlatform platform) : _URL(URL), _platform(platform) {}

SocialRecord::~SocialRecord() {}

void SocialRecord::Start()
{
	if (_startDate.empty()) // only allow once
	{
		// Store start date
		_startDate = GetDate();

		// Store start time for duration estimation
		_startTime = std::chrono::system_clock::now();

		// Set to writeable
		_writeable = true;
	}
}

void SocialRecord::End()
{
	if (!_startDate.empty() && _endDate.empty()) // only allow once and after start was called 
	{
		// Store end date
		_endDate = GetDate();

		// Store end time for duration estimation
		_endTime = std::chrono::system_clock::now();

		// Set to not writeable
		_writeable = false;
	}
}

void SocialRecord::Persist()
{
	// Check whether it is ok to persist if it is an unknown social platform aka normal webpage
	if (!setup::SOCIAL_RECORD_PERSIST_UNKNOWN && _platform == SocialPlatform::Unknown)
	{
		return;
	}

	// Convert record to JSON
	auto record = ToJSON(); 

	// Get keys of Firebase
	FirebaseIntegerKey countKey;
	FirebaseJSONKey recordKey;
	std::tie(countKey, recordKey) = SocialFirebaseKeys.at(_platform);

	// Persist
	std::promise<int> sessionPromise; auto sessionFuture = sessionPromise.get_future(); // prepare to get value
	FirebaseMailer::Instance().PushBack_Transform(countKey, 1, &sessionPromise); // adds one to the count
	std::string sessionString = std::to_string(sessionFuture.get() - 1); // start session indices at zero (blocks thread until get was executed but simpler than async call)
	sessionString = "session " + std::string((unsigned int)glm::max(setup::SOCIAL_RECORD_SESSION_DIGIT_COUNT - (int)sessionString.length(), 0), '0') + sessionString; // preceeding zeros for string
	FirebaseMailer::Instance().PushBack_Put(recordKey, record, sessionString); // send JSON to database
}

void SocialRecord::AddTimeInForeground(float time)
{
	if (_writeable) { _durationInForeground += time; }
}

void SocialRecord::AddTimeActiveUser(float time)
{
	if (_writeable) { _durationUserActive += time; }
}

void SocialRecord::AddScrollingDelta(float delta)
{
	if (_writeable) { _scrollAmount += delta; }
}

void SocialRecord::AddClick()
{
	if (_writeable) { _clickCount++; }
}

void SocialRecord::AddSubpage(std::string URL)
{
	if (_writeable) { _subpages.push_back(URL); }
}

nlohmann::json SocialRecord::ToJSON() const
{
	// Return JSON structure
	return {
		{ "url", GetURL() },
		{ "domain", GetDomain() },
		{ "startDate", _startDate },
		{ "endDate", _endDate },
		{ "duration", std::chrono::duration<float>(_endTime-_startTime).count() },
		{ "durationInForeground", _durationInForeground },
		{ "durationUserActive", _durationUserActive },
		{ "scrollAmount", _scrollAmount },
		{ "subpageCount", _subpages.size() },
		{ "subpages", _subpages },
		{ "clickCount", _clickCount }
	};
}

std::string SocialRecord::GetDate() const
{
	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);
	std::ostringstream oss;
	oss << std::put_time(&tm, DATE_FORMAT.c_str());
	return oss.str();
}