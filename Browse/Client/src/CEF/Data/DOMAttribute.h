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
const static std::string DOMAttrToString(DOMAttribute attr)
{
	switch (attr) {
	case Rects: return "Rects"; break;
	case FixedId: return "FixedId"; break;
	case OverflowId: return "OverflowId"; break;
	case Text: return "Text"; break;
	case IsPassword: return "IsPassword"; break;
	case Url: return "Url"; break;
	case Options: return "Options"; break;
	case MaxScrolling: return "MaxScrolling"; break;
	case CurrentScrolling: return "CurrentScrolling"; break;
	default: return std::to_string(attr);
	}
}


#endif // DOMATTRIBUTE_H_