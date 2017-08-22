//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Mueller (muellerd@uni-koblenz.de)
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// List of DOM node attributes.

#ifndef DOMATTRIBUTE_H_
#define DOMATTRIBUTE_H_

#include <string>

enum DOMAttribute {
	Rects, FixedId, OverflowId,
	Text, IsPassword, Url, Options,
	MaxScrolling, CurrentScrolling,
	OccBitmask
};

// Helper for debug output
std::string DOMAttrToString(DOMAttribute attr);

#endif // DOMATTRIBUTE_H_