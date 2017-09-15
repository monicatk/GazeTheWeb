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
#include <future>

using json = nlohmann::json;

// Keys (fill in mapping to strings below!)
enum class FirebaseIntegerKey { URL_INPUTS, MAX_BOOKMARK_COUNT, MAX_OPEN_TABS }; // TODO: these are (unused!) examples; TODO: divide between user and global Firebase Keys
enum class FirebaseStringKey { TEST_STRING };
enum class FirebaseJSONKey { TEST_JSON };

// Mapping from key to raw type
template<typename Type> struct FirebaseValue;
template<> struct FirebaseValue<FirebaseIntegerKey> { typedef int type; };
template<> struct FirebaseValue<FirebaseStringKey> { typedef std::string type; };
template<> struct FirebaseValue<FirebaseJSONKey> { typedef json type; };

// Firebase mailer class
class FirebaseMailer
{
private:

	// Fill mapping from FirebaseIntegerKey to string here!
	static const std::string BuildFirebaseKey(FirebaseIntegerKey key, std::string uid)
	{
		// Simple mapping of enum to string
		static const std::map<FirebaseIntegerKey, std::string> FirebaseKeyString // run time map, easier to implement than compile time template stuff
		{
			{ FirebaseIntegerKey::URL_INPUTS,			"urlInput" },
			{ FirebaseIntegerKey::MAX_BOOKMARK_COUNT,	"maxBookmarkCount" },
			{ FirebaseIntegerKey::MAX_OPEN_TABS,		"maxOpenTabs" },
		};

		return "users/" + uid + "/browse/" + FirebaseKeyString.at(key);
	}

	// Fill mapping from FirebaseStringKey to string here!
	static const std::string BuildFirebaseKey(FirebaseStringKey key, std::string uid)
	{
		// Simple mapping of enum to string
		static const std::map<FirebaseStringKey, std::string> FirebaseKeyString // run time map, easier to implement than compile time template stuff
		{
			{ FirebaseStringKey::TEST_STRING,			"testString" },
		};

		return "users/" + uid + "/browse/" + FirebaseKeyString.at(key);
	}

	// Fill mapping from FirebaseJSONKey to string here!
	static const std::string BuildFirebaseKey(FirebaseJSONKey key, std::string uid)
	{
		// Simple mapping of enum to string
		static const std::map<FirebaseJSONKey, std::string> FirebaseKeyString // run time map, easier to implement than compile time template stuff
		{
			{ FirebaseJSONKey::TEST_JSON,				"JSONString" },
		};

		return "users/" + uid + "/browse/" + FirebaseKeyString.at(key);
	}

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

	// Available commands (TODO: use more templates?)
	void PushBack_Login(std::string email, std::string password);
	void PushBack_Transform(FirebaseIntegerKey key, int delta, std::promise<int>* pPromise = nullptr); // promise delivers future database value
	void PushBack_Maximum(FirebaseIntegerKey key, int value, std::promise<int>* pPromise = nullptr); // promise delivers future database value
	void PushBack_Put(FirebaseIntegerKey key, int value);
	void PushBack_Put(FirebaseStringKey key, std::string value);
	void PushBack_Put(FirebaseJSONKey key, json value);
	void PushBack_Get(FirebaseIntegerKey key, std::promise<int>* pPromise);
	void PushBack_Get(FirebaseStringKey key, std::promise<std::string>* pPromise);
	void PushBack_Get(FirebaseJSONKey key, std::promise<json>* pPromise);

private:

	// ### Delegate running in a thread ###
	class FirebaseInterface
	{
	public:

		// Log in. Return whether successful
		bool Login(std::string email, std::string password);

		// Simple put functionality. Can replace existing value
		template<typename T>
		void Put(T key, typename FirebaseValue<T>::type value);

		// Get
		template<typename T>
		void Get(T key, std::promise<typename FirebaseValue<T>::type>* pPromise);

		// Transform value
		void Transform(FirebaseIntegerKey key, int delta, std::promise<int>* pPromise = nullptr); // if nullptr, no future is set

		// Save maximum in database, either my value or the one in database
		void Maximum(FirebaseIntegerKey key, int value, std::promise<int>* pPromise = nullptr); // if nullptr, no future is set

	private:

		// Relogin via refresh token. Returns whether successful
		bool Relogin();

		// Put single value in database. Uses ETag to check whether ok to put or working on outdated value. Return true if succesful, otherwise do it another time, maybe with new value provided by this method
		template<typename T>
		bool Put(T key, std::string ETag, typename FirebaseValue<T>::type value, std::string& rNewETag, typename FirebaseValue<T>::type& rNewValue); // Fills newETag and newValue if ETag was outdated. Please try again with these values

		// Get of JSON structure by key. Returns empty structure (pair of ETag and JSON encoded data) if not available
		std::pair<std::string, json> Get(std::string key);

		// Apply function on value in database. Returns final value in database
		int Apply(FirebaseIntegerKey key, std::function<int(int)> function); // function parameter takes value from database on which function is applied. Return value is then written to database

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