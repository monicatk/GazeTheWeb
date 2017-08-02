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

enum class FirebaseValue { PAGE_VISITS };

// TODO
// - relogin when token is invalid (at least one atempt)
// - send / receive data, maybe some intelligent value incrementation
// - right now, no header evaluation is performed. just the data parsed
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
	json Get(std::string key);

	// Destructor
	~FirebaseMailer() {}

private:

	// Constants
	const std::string API_KEY = "AIzaSyBoySYE4mQVhrtCB_1TbPsXa86W8_y35Ug"; // API key for our Firebase
	const std::string FIREBASE_URL = "https://hellofirebase-2d544.firebaseio.com"; // URL of our Firebase
	// TODO: at least store refresh token to relogin after timeout of token
	
	// Members
	std::string _token = "";

	// Private copy / assignment constructors
	FirebaseMailer() {};
	FirebaseMailer(const FirebaseMailer&) {}
	FirebaseMailer& operator = (const FirebaseMailer &) { return *this; }
};

#endif // FIREBASEMAILER_H_