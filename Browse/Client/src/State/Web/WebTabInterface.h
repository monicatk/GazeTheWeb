//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// Interface how Tab can interact with Web class.

#ifndef WEBTABINTERFACE_H_
#define WEBTABINTERFACE_H_

#include <string>

// Forward declaration
class Tab;

class WebTabInterface
{
public:

	// Add tab after that tab
	virtual void AddTabAfter(Tab* caller, std::string URL) = 0;

};

#endif // WEBTABINTERFACE_H_
