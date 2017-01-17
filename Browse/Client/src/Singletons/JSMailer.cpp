//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "JSMailer.h"
#include "src/CEF/Handler.h"

void JSMailer::SetHandler(Handler* pHandler)
{
	instance()._pHandler = pHandler;
}

void JSMailer::Send(std::string message)
{
	if (instance()._pHandler != nullptr)
	{
		instance()._pHandler->SendToJSLoggingMediator(message);
	}
}