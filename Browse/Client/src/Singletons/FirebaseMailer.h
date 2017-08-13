//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Singleton which receives and sends data to Firebase.

#ifndef FIREBASEMAILER_H_
#define FIREBASEMAILER_H_

#include "src/Setup.h"
#include "submodules/json/src/json.hpp"
#include <string>

using json = nlohmann::json;

enum class FirebaseKey { URL_INPUTS, MAX_BOOKMARK_COUNT, MAX_OPEN_TABS };
const std::map<FirebaseKey, std::string> FirebaseKeyString // run time map, easier to implement than compile time template stuff
{
	{ FirebaseKey::URL_INPUTS,			"test/URL_INPUT" },
	{ FirebaseKey::MAX_BOOKMARK_COUNT,	"test/MAX_BOOKMARK_COUNT" },
	{ FirebaseKey::MAX_OPEN_TABS,		"test/MAX_OPEN_TABS" },
};

// TODO
// - Make a command queue and put everything in a single separate thread
// - Make is pausable (aka "Pause Data Transfer") -> can be done in command queue level
// - Probably user name necessary to specify exact path the may write and read -> function that combines both, so name can be saved as member here (or via uid?)

class FirebaseMailer
{
public:

	// Getter of static instance
	static FirebaseMailer& Instance()
	{
		static FirebaseMailer _instance;
		return _instance;
	}

	// Log in. Return whether successful
	bool Login(std::string email, std::string password);

	// Get of JSON structure by key. Returns empty structure (pair of ETag and JSON encoded data) if not available
	std::pair<std::string, json> Get(FirebaseKey key);
	std::pair<std::string, json> Get(std::string key);

	// Transform value
	void Transform(FirebaseKey key, int delta);

	// Save maximum in database, either my value or the one in database
	void Maximum(FirebaseKey key, int value);

	// Destructor
	~FirebaseMailer() {}

private:

	// Relogin via refresh token. Returns whether successful
	bool Relogin();

	// Apply function on value in database
	void Apply(FirebaseKey key, std::function<int(int)>); // function parameter takes value from database on which function is applied. Return value is then written to database

	// Constants
	const std::string API_KEY = setup::FIREBASE_API_KEY;
	const std::string URL = setup::FIREBASE_URL;

	// Members
	std::string _idToken = ""; // short living token for identifying (indicator for being logged in!)
	std::string _refreshToken = ""; // long living token for refreshing itself and idToken

	// Private copy / assignment constructors
	FirebaseMailer() {};
	FirebaseMailer(const FirebaseMailer&) {}
	FirebaseMailer& operator = (const FirebaseMailer &) { return *this; }
};

#endif // FIREBASEMAILER_H_