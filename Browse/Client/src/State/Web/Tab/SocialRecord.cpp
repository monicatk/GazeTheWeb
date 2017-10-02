//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "SocialRecord.h"
#include <iomanip>
#include <ctime>
#include <sstream>

using json = nlohmann::json;

SocialRecord::SocialRecord(std::string domain, SocialPlatform platform) : _domain(domain), _platform(platform) {}

SocialRecord::~SocialRecord() {}

void SocialRecord::StartAndAddPage(std::string URL)
{
	if (_startDate.empty()) // only allow once
	{
		// Store start date
		_startDate = GetDate();

		// Store start time for duration estimation
		_startTime = std::chrono::system_clock::now();

		// Set to writeable
		_writeable = true;

		// Add page
		AddPage(URL); // otherwise page vector would be empty and any attempt of updating would throw exception
	}
}

void SocialRecord::End()
{
	if (!_startDate.empty() && _endDate.empty()) // only allow once and after start was called 
	{
		// Set duration of last page
		if (!_pages.empty())
		{
			_pages.back().duration = std::chrono::duration<float>(std::chrono::system_clock::now() - _lastAddPage).count();
		}

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
	FirebaseMailer::Instance().PushBack_Put(recordKey, record, "sessions/" + std::to_string(sessionFuture.get() - 1)); // send JSON to database
}

void SocialRecord::AddTimeInForeground(float time)
{
	if (_writeable) { _totalDurationInForeground += time; _pages.back().durationInForeground += time; }
}

void SocialRecord::AddTimeActiveUser(float time)
{
	if (_writeable) { _totalDurationUserActive += time; _pages.back().durationUserActive += time; }
}

void SocialRecord::AddScrollingDelta(float delta)
{
	if (_writeable) { _pages.back().scrollAmount += delta; }
}

void SocialRecord::AddClick(std::string tag, std::string id, float x, float y)
{
	if (_writeable) { _pages.back().clicks.push_back(Click(tag, id, x, y, std::chrono::duration<double>(std::chrono::system_clock::now() - _startTime).count())); }
}

void SocialRecord::AddTextInput(std::string id, int charCount, int charDistance, float x, float y, float duration)
{
	if (_writeable) { _pages.back().textInputs.push_back(TextInput(id, charCount, charDistance, x, y, std::chrono::duration<double>(std::chrono::system_clock::now() - _startTime).count(), duration)); }
}

void SocialRecord::AddPage(std::string URL)
{
	if (_writeable)
	{
		// Set duration of previous page
		if (!_pages.empty())
		{
			_pages.back().duration = std::chrono::duration<float>(std::chrono::system_clock::now() - _lastAddPage).count();
		}
		_lastAddPage = std::chrono::system_clock::now(); // store time of adding the page

		// Add new page
		_pages.push_back(Page(URL));
	}
}

nlohmann::json SocialRecord::ToJSON() const
{
	// Domain JSON structure
	json JSON = {
		{ "domain", GetDomain() },
		{ "startDate", _startDate },
		{ "endDate", _endDate },
		{ "duration", std::chrono::duration<float>(_endTime-_startTime).count() },
		{ "durationInForeground", _totalDurationInForeground },
		{ "durationUserActive", _totalDurationUserActive },
		{ "pageCount", _pages.size() },
	};

	// Add pages to JSON structure
	std::vector<json> pagesJSON;
	for (const auto& rPage : _pages)
	{
		JSON["pages"].push_back(
		{
			{ "url",  rPage.URL },
			{ "duration", rPage.duration },
			{ "durationInForeground",  rPage.durationInForeground },
			{ "durationUserActive",  rPage.durationUserActive },
			{ "scrollAmount",  rPage.scrollAmount },
			{ "clickCount",  rPage.clicks.size() },
			{ "textInputCount",  rPage.textInputs.size() }
		});

		// Add clicks to JSON structure of page
		for (const auto& rClick : rPage.clicks)
		{
			JSON["pages"].back()["clicks"].push_back(
			{
				{ "tag", rClick.tag },
				{ "id", rClick.id },
				{ "coord", std::to_string(rClick.x) + ", " + std::to_string(rClick.y) },
				{ "time", rClick.time }
			}
			);
		}

		// Add text inputs to JSON structure of page
		for (const auto& rTextInput : rPage.textInputs)
		{
			JSON["pages"].back()["textInputs"].push_back(
			{
				{ "id", rTextInput.id },
				{ "charCount", rTextInput.charCount },
				{ "charDistance", rTextInput.charDistance },
				{ "coord", std::to_string(rTextInput.x) + ", " + std::to_string(rTextInput.y) },
				{ "time", rTextInput.time },
				{ "duration", rTextInput.duration }
			}
			);
		}
	}

	// Return result
	return JSON;
}

std::string SocialRecord::GetDate() const
{
	auto t = std::time(nullptr);
	auto tm = *std::localtime(&t);
	std::ostringstream oss;
	oss << std::put_time(&tm, DATE_FORMAT.c_str());
	return oss.str();
}

std::string SocialRecord::PrecedeZeros(const std::string& rInput, const int digitCount) const
{
	return std::string((unsigned int)glm::max(digitCount - (int)rInput.length(), 0), '0') + rInput; // preceding zeros for string
}