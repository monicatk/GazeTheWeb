//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Mueller (muellerd@uni-koblenz.de)
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#ifndef DOMATTRIBUTE_H_
#define DOMATTRIBUTE_H_

#include <string>

enum DOMAttribute {
	Rects = 0, FixedId, OverflowId,
	Text, IsPassword, Url, Options,
	MaxScrolling, CurrentScrolling
};

// Helper for debug output
std::string DOMAttrToString(DOMAttribute attr);

#endif // DOMATTRIBUTE_H_