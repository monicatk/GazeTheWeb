//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Singleton which receives and sends data to Firebase.

#ifndef FIREBASEMAILER_H_
#define FIREBASEMAILER_H_

#include "submodules/json/src/json.hpp"
#include <string>

using json = nlohmann::json;

enum class FirebaseKey { URL_INPUTS, MAX_BOOKMARK_COUNT, MAX_OPEN_TABS };
const std::map<FirebaseKey, std::string> FirebaseKeyString
{
	{ FirebaseKey::URL_INPUTS,			"URL_INPUT" },
	{ FirebaseKey::MAX_BOOKMARK_COUNT,	"MAX_BOOKMARK_COUNT" },
	{ FirebaseKey::MAX_OPEN_TABS,		"MAX_OPEN_TABS" },
};

// TODO
// - refresh token mechanism
// - Make a command queue and put everything in a single separate thread

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

	// Constants
	const std::string API_KEY = "AIzaSyBoySYE4mQVhrtCB_1TbPsXa86W8_y35Ug"; // API key for our Firebase
	const std::string FIREBASE_URL = "https://hellofirebase-2d544.firebaseio.com"; // URL of our Firebase

	// Members
	std::string _token = "";

	// Private copy / assignment constructors
	FirebaseMailer() {};
	FirebaseMailer(const FirebaseMailer&) {}
	FirebaseMailer& operator = (const FirebaseMailer &) { return *this; }
};

#endif // FIREBASEMAILER_H_