//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Mueller (muellerd@uni-koblenz.de)
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================
// - lists available DOM node attributes
// - defines mapping from integer to DOMAttribute and 
// - DOMAttribute to readable string

#ifndef DOMATTRIBUTE_H_
#define DOMATTRIBUTE_H_

#include <string>

// TODO: Provide DOMAttribute >>objects<< which store respective data and a string representation of stored data?

enum DOMAttribute {
	Rects, 
	FixedId, 
	OverflowId,
	Text, 
	IsPassword, 
	Url, 
	Options,
	MaxScrolling, 
	CurrentScrolling,
	OccBitmask
};

// Helper for debug output
std::string DOMAttrToString(DOMAttribute attr);

// Straight forward mapping from integer to enum
static DOMAttribute IntToDOMAttribute(int const& val)
{
	return static_cast<DOMAttribute>(val);
}


#endif // DOMATTRIBUTE_H_