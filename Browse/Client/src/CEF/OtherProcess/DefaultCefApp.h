//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Mueller (muellerd@uni-koblenz.de)
//============================================================================

#ifndef CEF_DEFAULTCEFAPP_H_
#define CEF_DEFAULTCEFAPP_H_

#include "include/cef_app.h"

class DefaultCefApp : public CefApp
{
public:
	DefaultCefApp() {}

private:
	IMPLEMENT_REFCOUNTING(DefaultCefApp);
	DISALLOW_COPY_AND_ASSIGN(DefaultCefApp);
};

#endif // CEF_DEFAULTCEFAPP_H_