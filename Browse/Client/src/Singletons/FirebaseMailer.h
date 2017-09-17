//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Singleton which receives and sends data to Firebase. The mailer manages
// the command queue and the interface the connection to the Firebase.

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

// Available database keys
enum class FirebaseIntegerKey	{ URL_INPUTS, MAX_BOOKMARK_COUNT, MAX_OPEN_TABS };
enum class FirebaseStringKey	{ TEST_STRING };
enum class FirebaseJSONKey		{ TEST_JSON };

// Mapping from key to raw type
template<typename Type> struct FirebaseValue;
template<> struct FirebaseValue<FirebaseIntegerKey>	{ typedef int type; };
template<> struct FirebaseValue<FirebaseStringKey>	{ typedef std::string type; };
template<> struct FirebaseValue<FirebaseJSONKey>	{ typedef json type; };

// Mapping from key to address
template<typename T> static std::string FirebaseAddress(T key);
template<> std::string FirebaseAddress<FirebaseIntegerKey>(FirebaseIntegerKey key)
{
	switch (key)
	{
	case FirebaseIntegerKey::URL_INPUTS:
		return "urlInput";
	case FirebaseIntegerKey::MAX_BOOKMARK_COUNT:
		return "maxBookmarkCount";
	case FirebaseIntegerKey::MAX_OPEN_TABS:
		return "maxOpenTabs";
	default: return "";
	}
};
template<> std::string FirebaseAddress<FirebaseStringKey>(FirebaseStringKey key)
{
	switch (key)
	{
	case FirebaseStringKey::TEST_STRING:
		return "testString";
	default: return "";
	}
};
template<> std::string FirebaseAddress<FirebaseJSONKey>(FirebaseJSONKey key)
{
	switch (key)
	{
	case FirebaseJSONKey::TEST_JSON:
		return "JSONString";
	default: return "";
	}
};

// Firebase mailer class
class FirebaseMailer
{
private:

	// Address creation
	template<typename T>
	static const std::string BuildFirebaseKey(T key, std::string uid)
	{
		return "users/" + uid + "/browse/" + FirebaseAddress<T>(key);
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

	// Available commands
	void PushBack_Login		(std::string email, std::string password);
	void PushBack_Transform	(FirebaseIntegerKey key, int delta, std::promise<int>* pPromise = nullptr); // promise delivers future database value
	void PushBack_Maximum	(FirebaseIntegerKey key, int value, std::promise<int>* pPromise = nullptr); // promise delivers future database value
	void PushBack_Put		(FirebaseIntegerKey key, int value);
	void PushBack_Put		(FirebaseStringKey key, std::string value);
	void PushBack_Put		(FirebaseJSONKey key, json value);
	void PushBack_Get		(FirebaseIntegerKey key, std::promise<int>* pPromise);
	void PushBack_Get		(FirebaseStringKey key, std::promise<std::string>* pPromise);
	void PushBack_Get		(FirebaseJSONKey key, std::promise<json>* pPromise);

private:

	// ### Delegate running in a thread ###
	class FirebaseInterface
	{
	public:

		// Log in. Return whether successful
		bool Login(std::string email, std::string password);

		// Simple put functionality. Replaces existing value if available
		template<typename T>
		void Put(T key, typename FirebaseValue<T>::type value); // delegates private put

		// Get
		template<typename T>
		void Get(T key, std::promise<typename FirebaseValue<T>::type>* pPromise); // delegates private get

		// Transform value
		void Transform(FirebaseIntegerKey key, int delta, std::promise<int>* pPromise = nullptr); // if nullptr, no future is set

		// Save maximum in database, either my value or the one in the database
		void Maximum(FirebaseIntegerKey key, int value, std::promise<int>* pPromise = nullptr); // if nullptr, no future is set

	private:

		// Struct for ETag and database value
		struct DBEntry
		{
			DBEntry() {};
			DBEntry(std::string ETag, json value) : ETag(ETag), value(value) {};
			std::string ETag = "";
			json value;
		};

		// Relogin via refresh token. Returns whether successful
		bool Relogin();

		// Put single value in database. Uses ETag to check whether ok to put or working on outdated value. Return true if successful, otherwise do it another time, maybe with new value provided by this method
		template<typename T> // TODO: here on could get rid of templates and just work on JSON objects
		bool Put(T key, std::string ETag, typename FirebaseValue<T>::type value, std::string& rNewETag, typename FirebaseValue<T>::type& rNewValue); // fills newETag and newValue if ETag was outdated. Please try again with these values

		// Get of JSON structure by key. Returns empty structure if not available
		DBEntry Get(std::string key);

		// Apply function on value in database. Returns final value that is stored in database
		int Apply(FirebaseIntegerKey key, std::function<int(int)> function); // function parameter takes value from database on which function is applied. Its return value is then written to database

		// Constants
		const std::string _API_KEY = setup::FIREBASE_API_KEY;
		const std::string _URL = setup::FIREBASE_URL;

		// Members
		std::string _idToken = ""; // short living token for identification (indicator for being logged in!)
		std::string _refreshToken = ""; // long living token for refreshing itself and idToken
		std::string _uid = "0"; // user identifier (initialized with something that indicates "broken")
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

	// Pause indicator (if true, avoids pushing to command queue)
	bool _paused = false;

	// Threading (thread defined in constructor of FirebaseMailer)
	std::mutex _mutex; // mutex for access of _commandQueue (thread grabs all commands and works on them)
	std::condition_variable _conditionVariable; // used to wake up thread at available work
	std::deque<std::shared_ptr<Command> > _commandQueue; // shared function pointers that are executed sequentially within thread
	std::unique_ptr<std::thread> _upThread; // the thread itself
	bool _shouldStop = false; // written by this, read by thread
};

#endif // FIREBASEMAILER_H_