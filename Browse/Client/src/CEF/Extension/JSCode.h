//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Müller (muellerd@uni-koblenz.de)
//============================================================================

#ifndef CEF_JSCODE_H_
#define CEF_JSCODE_H_

#include <string>
#include <map>
#include <sstream>
#include <fstream>
#include <iostream>

/**
*	This class loads externally saved Javascript code from .js files in CONTENT_PATH/javascript
*	and returns them as std::string
*	USAGE GUIDE:
*	1.)	Save your Javascript code to the mentioned file location
*	2.) Add a corresponding enum name to JSFile enum
*	3.)	Add a pair (JSFile, file name) to std::map findJSFile
*	4.) Include JSCode.h to your C++ class
*	5.) Add a constant std::string member to your class and initialize it with GetJSCode function
*	6.)	Inject your Javascript code by using your new member variable
*
*/

enum JSFile
{
    REMOVE_CSS_SCROLLBAR,
    DOM_UPDATE_SIZES,
    DOM_FILL_ARRAYS,
    FAVICON_GET_URL_AND_RESOLUTION,
    FAVICON_CREATE_IMG,
    FAVICON_COPY_IMG_BYTES_TO_V8ARRAY,
    MUTATION_OBSERVER_TEST,
    FIXED_ELEMENT_SEARCH,
    FIXED_ELEMENT_READ_OUT,
	DOM_MUTATIONOBSERVER,
	DOM_FIXED_ELEMENTS
};

std::string GetJSCode(JSFile file);

// TODO: Create solution for functions with input parameters

// Functions
std::string jsInputTextData(int inputID, std::string text, bool submit=false);
std::string jsFavIconUpdate(std::string oldUrl);

#endif // CEF_JSCODE_H_
