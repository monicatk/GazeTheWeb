//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Master interface for notifications. TODO: make notification disappear when fixation leaves it

#ifndef MASTERNOTIFICATIONINTERFACE_H_
#define MASTERNOTIFICATIONINTERFACE_H_

#include <string>

class MasterNotificationInterface
{
public:

	// Enum about notification types
	enum class Type { NEUTRAL, SUCCESS, WARNING };

	// Push notification to display
	virtual void PushNotification(std::u16string content, Type type, bool overridable) = 0;

	// Push notification to display taken from localization file
	virtual void PushNotificationByKey(std::string key, Type type, bool overridable) = 0;
};

#endif // MASTERNOTIFICATIONINTERFACE_H_
