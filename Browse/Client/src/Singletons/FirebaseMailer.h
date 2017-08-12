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
	{ FirebaseKey::URL_INPUTS,			"URL_INPUT" },
	{ FirebaseKey::MAX_BOOKMARK_COUNT,	"MAX_BOOKMARK_COUNT" },
	{ FirebaseKey::MAX_OPEN_TABS,		"MAX_OPEN_TABS" },
};

// TODO
// - refresh token mechanism (for this, every answer header must be parsed and checked for the return value)
// - Make a command queue and put everything in a single separate thread
// - Make is pausable (aka "Pause Data Transfer")

class FirebaseMailer
{
public:

	// Getter of static instance
	static FirebaseMailer& Instance()
	{
		static FirebaseMailer _instance;
		return _instance;
	}

	// Log in
	bool Login(std::string email, std::string password);

	// Get of JSON structure by key. Returns empty structure if not available
	json Get(FirebaseKey key);
	json Get(std::string key);

	// Transform value
	void Transform(FirebaseKey key, int delta);

	// Save maximum in database, either my value or the one in database
	void Maximum(FirebaseKey key, int value);

	// Destructor
	~FirebaseMailer() {}

private:

	// Apply function on value in database
	void Apply(FirebaseKey key, std::function<int(int)>); // function parameter takes value from database on which function is applied. Return value is then written to database

	// Constants
	const std::string API_KEY = setup::FIREBASE_API_KEY;
	const std::string URL = setup::FIREBASE_URL;

	// Members
	std::string _token = "";

	// Private copy / assignment constructors
	FirebaseMailer() {};
	FirebaseMailer(const FirebaseMailer&) {}
	FirebaseMailer& operator = (const FirebaseMailer &) { return *this; }
};

#endif // FIREBASEMAILER_H_