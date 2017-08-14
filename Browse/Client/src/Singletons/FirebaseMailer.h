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
#include <deque>
#include <thread>
#include <mutex>
#include <condition_variable>

using json = nlohmann::json;

enum class FirebaseKey { URL_INPUTS, MAX_BOOKMARK_COUNT, MAX_OPEN_TABS }; // TODO: these are (unused!) examples; TODO: divide between user and global Firebase Keys
static std::string BuildFirebaseKey(FirebaseKey key, std::string uid)
{
	// Simple mapping of enum to string
	static const std::map<FirebaseKey, std::string> FirebaseKeyString // run time map, easier to implement than compile time template stuff
	{
		{ FirebaseKey::URL_INPUTS,			"urlInput" },
		{ FirebaseKey::MAX_BOOKMARK_COUNT,	"maxBookmarkCount" },
		{ FirebaseKey::MAX_OPEN_TABS,		"maxOpenTabs" },
	};

	return "users/" + uid + "/browse/" + FirebaseKeyString.at(key);
}

// TODO
// - Implement Get command that returns a future

class FirebaseMailer
{
public:

	// Getter of static instance
	static FirebaseMailer& Instance()
	{
		static FirebaseMailer _instance;
		return _instance;
	}

	// Destructor
	~FirebaseMailer() { _shouldStop = true; _conditionVariable.notify_all(); _upThread->join(); } // tell thread to stop

	// Continue mailer
	void Continue() { _paused = false; }

	// Pause mailer
	void Pause() { _paused = true; }

	// Available commands
	void PushBack_Login(std::string email, std::string password);
	void PushBack_Transform(FirebaseKey key, int delta);
	void PushBack_Maximum(FirebaseKey key, int value);

private:

	// ### Delegate running a thread ###
	class FirebaseInterface
	{
	public:

		// Log in. Return whether successful
		bool Login(std::string email, std::string password);

		// Get of JSON structure by key. Returns empty structure (pair of ETag and JSON encoded data) if not available
		std::pair<std::string, json> Get(FirebaseKey key);
		std::pair<std::string, json> Get(std::string key);

		// Put single value in database. Uses ETag to check whether ok to put or worked on outdated value. Return true if succesful, otherwise do it another time, maybe with new value
		bool Put(FirebaseKey key, std::string ETag, int value, std::string& rNewETag, int& rNewValue); // Fills newETag and newValue if ETag was outdated. Please try again with these values

		// Transform value
		void Transform(FirebaseKey key, int delta);

		// Save maximum in database, either my value or the one in database
		void Maximum(FirebaseKey key, int value);

	private:

		// Relogin via refresh token. Returns whether successful
		bool Relogin();

		// Apply function on value in database
		void Apply(FirebaseKey key, std::function<int(int)> function); // function parameter takes value from database on which function is applied. Return value is then written to database

		// Constants
		const std::string _API_KEY = setup::FIREBASE_API_KEY;
		const std::string _URL = setup::FIREBASE_URL;

		// Members
		std::string _idToken = ""; // short living token for identifying (indicator for being logged in!)
		std::string _refreshToken = ""; // long living token for refreshing itself and idToken
		std::string _uid = "0"; // user identifier (initialize with something that indicates "broken")
	};
	// #################################

	// Typdef of command
	typedef const std::function<void(FirebaseInterface&)> Command;

	// Method that actually pushes back command (considers whether paused etc.)
	void PushBackCommand(std::shared_ptr<Command> spCommand);

	// Private copy / assignment constructors
	FirebaseMailer();
	FirebaseMailer(const FirebaseMailer&) {}
	FirebaseMailer& operator = (const FirebaseMailer &) { return *this; }

	// Pause indicator
	bool _paused = false;

	// Threading
	std::mutex _mutex; // mutex for access of _commandQueue (thread grabs all commands and works on them)
	std::condition_variable _conditionVariable; // used to wake up thread at available work
	std::deque<std::shared_ptr<Command> > _commandQueue; // shared function pointers that are executed sequentially within thread
	std::unique_ptr<std::thread> _upThread; // the thread itself
	bool _shouldStop = false; // written by this, read by thread
};

#endif // FIREBASEMAILER_H_