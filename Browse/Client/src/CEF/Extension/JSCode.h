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
    FIXED_ELEMENT_READ_OUT
};

std::string GetJSCode(JSFile file);

// TODO: Create solution for functions with input parameters

// Functions
std::string jsInputTextData(int inputID, std::string text, bool submit=false);
std::string jsFavIconUpdate(std::string oldUrl);

/*
const std::string JS_REMOVE_CSS_SCROLLBAR = "\
    var css = document.createElement('style');\
    css.type = 'text/css';\
    css.innerHTML = 'body::-webkit-scrollbar { width:0px !important; }';\
    document.getElementsByTagName(\"head\")[0].appendChild(css);"
    ;
const std::string JS_REMOVE_HTML_SCROLLBAR = "\
    var divs = document.getElementsByTagName('div');\
    for (i = 0; i < divs.length; i++)\
    {\
        if(divs[i].hasAttribute('style'))\
            if(divs[i].style.hasAttribute('overflow'))\
                divs[i].style.overflow = 'hidden';\
    }\
    document.body.style.scroll='no'"
    ;

const std::string JS_UPDATE_SIZE_TEXTINPUTS = "\
    var inputNodes = document.getElementsByTagName('input');\
    var textInput = [];\
    for (i = 0; i < inputNodes.length; i++)\
        if(inputNodes[i].type == 'text' || inputNodes[i].type == 'search' || inputNodes[i].type == 'email' || inputNodes[i].type == 'password')\
        {\
            var rect = inputNodes[i].getBoundingClientRect();\
            if(rect.width > 0 && rect.height > 0)\
            {\
                if(textInput.length > 0)\
                {\
                    var rect2 = textInput[textInput.length-1].getBoundingClientRect();\
                    if(rect2.top != rect.top || rect2.bottom != rect.bottom || rect2.left != rect.left || rect2.right != rect.right)\
                    {\
                        textInput.push(inputNodes[i]);\
                    }\
                }\
                else\
                {\
                    textInput.push(inputNodes[i]);\
                }\
            }\
        }\
    window.sizeTextInputs = textInput.length;"
    ;

const std::string JS_FILL_ARRAY_TEXTINPUTS = "\
    var doc = document.documentElement;\
    for(i = 0; i < textInput.length; i++)\
    {\
        var offsetX = (window.pageXOffset || doc.scrollLeft) - (doc.clientLeft || 0); \
        var offsetY = (window.pageYOffset || doc.scrollTop) - (doc.clientTop || 0); \
        var rect = textInput[i].getBoundingClientRect();\
        window.TextInputs[i].coordinates = [rect.top + offsetY, rect.left + offsetX, rect.bottom + offsetY, rect.right + offsetX];\
        window.TextInputs[i].value = textInput[i].value;\
    }"
    ;

const std::string JS_SET_FAVICON_URL_RESOLUTION =
    //alert('favIcon main script');
    "var links = document.getElementsByTagName('link');\
    window.favIconUrl = '';\
    for (i = 0; i < links.length; i++)\
    {\
        if(links[i].rel == 'icon' || links[i].rel == 'shortcut icon')\
        {\
            window.favIconUrl = links[i].href;\
        }\
    }"
    //if(window.favIconUrl == '')\
    //	alert('(main) Searched '+links.length+', have not found iconURL...');
    "var favIconImg = new Image();\
    var canvas = document.createElement('canvas');\
    var ctx = canvas.getContext('2d');\
    favIconImg.onload = function(){\
        canvas.width = favIconImg.width;\
        canvas.height = favIconImg.height;\
        ctx.drawImage(favIconImg, 0, 0);\
        window.favIconHeight = canvas.height;\
        window.favIconWidth = canvas.width;"
        //alert('(main/update) Updated favIconImg!');
    "};\
    favIconImg.src = window.favIconUrl;\
    "
    // 		alert('URL: '+window.favIconUrl+', favIconWidth: '+window.favIconWidth+', favIconHeight: '+window.favIconHeight);
    ;
*/

#endif // CEF_JSCODE_H_
