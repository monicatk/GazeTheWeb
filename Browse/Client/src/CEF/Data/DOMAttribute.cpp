//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Mueller (muellerd@uni-koblenz.de)
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "DOMAttribute.h"

// Helper for debug output
std::string DOMAttrToString(DOMAttribute attr)
{
	switch (attr) {
	case Rects:				return "Rects"; break;
	case FixedId:			return "FixedId"; break;
	case OverflowId:		return "OverflowId"; break;
	case Text:				return "Text"; break;
	case IsPassword:		return "IsPassword"; break;
	case Url:				return "Url"; break;
	case Options:			return "Options"; break;
	case MaxScrolling:		return "MaxScrolling"; break;
	case CurrentScrolling:	return "CurrentScrolling"; break;
	default:				return std::to_string(attr);
	}
}