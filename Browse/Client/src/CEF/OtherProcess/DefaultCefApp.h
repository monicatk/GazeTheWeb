//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// CefApp implementation for all processes but Main and Render.

#ifndef CEF_DEFAULTCEFAPP_H_
#define CEF_DEFAULTCEFAPP_H_

#include "include/cef_app.h"

class DefaultCefApp : public CefApp
{
public:

	// Constructor
	DefaultCefApp() {}

private:

	// Include CEF'S default reference counting implementation
	IMPLEMENT_REFCOUNTING(DefaultCefApp);
	DISALLOW_COPY_AND_ASSIGN(DefaultCefApp);
};

#endif // CEF_DEFAULTCEFAPP_H_