//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Daniel Müller (muellerd@uni-koblenz.de)
//============================================================================

#include "JSCode.h"

const std::string src = CONTENT_PATH "/javascript/";

// List of all available JS code file paths and how to access them with JSCode enum
const std::map<JSFile, std::string> findJSFile =
{
    std::make_pair<JSFile, std::string>(REMOVE_CSS_SCROLLBAR, src + "remove_css_scrollbar.js"),
    std::make_pair<JSFile, std::string>(DOM_UPDATE_SIZES, src + "dom_update_sizes.js"),
    std::make_pair<JSFile, std::string>(DOM_FILL_ARRAYS, src + "dom_fill_arrays.js"),
    std::make_pair<JSFile, std::string>(FAVICON_GET_URL_AND_RESOLUTION, src + "favicon_get_url_and_resolution.js"),
    std::make_pair<JSFile, std::string>(FAVICON_CREATE_IMG, src + "favicon_create_img.js"),
    std::make_pair<JSFile, std::string>(FAVICON_COPY_IMG_BYTES_TO_V8ARRAY, src + "favicon_copy_img_bytes_to_v8array.js"),
    std::make_pair<JSFile, std::string>(MUTATION_OBSERVER_TEST, src + "mutation_observer_test.js"),
    std::make_pair<JSFile, std::string>(FIXED_ELEMENT_SEARCH, src + "fixed_element_search.js"),
    std::make_pair<JSFile, std::string>(FIXED_ELEMENT_READ_OUT, src + "fixed_element_read_out.js"),
	std::make_pair<JSFile, std::string>(DOM_MUTATIONOBSERVER, src + "dom_mutationobserver.js"
};

std::string GetJSCode(JSFile file)
{
    if (findJSFile.find(file) != findJSFile.end())
    {
        const std::string filePath = findJSFile.at(file);

        // Write file data to String, source: http://stackoverflow.com/questions/2602013/read-whole-ascii-file-into-c-stdstring

        std::ifstream t(filePath);
        if (t.is_open())
        {
            t.seekg(0, std::ios::end);
            size_t size = t.tellg();
            std::string buffer(size, ' ');
            t.seekg(0);
            t.read(&buffer[0], size);
            return buffer;
        }
        else
        {
            std::cout << "ERROR: Couldn't open JS file!" << std::endl; // TODO: couts do not appear in console!
            return "alert('ERROR: Could not open JS code file!');";
        }
    }
    else
    {
        std::cout << "ERROR: JSCode has not found the requested code file!" << std::endl;
        return "alert('ERROR: JSCode has not found the requested code file!');";
    }
}

// TODO: Delete this method and use CefV8Value::ExecuteFunction(WithContext)? Possible in Browser Thread without context->Enter()?
std::string jsInputTextData(int inputID, std::string text, bool submit)
{
    std::string code = "var input = window.dom_textinputs[" + std::to_string(inputID) + "];\
                        input.setAttribute('value','" + text + "');"
    ;

    if (submit)
    {
    code +=	"var parent = input.parentNode;\
            while(parent.nodeName != 'FORM')\
            {\
                parent = parent.parentNode;\
                if(parent === document.documentElement)\
                {\
                    alert('Button is no child of any form! Can not submit anything.');\
                    break;\
                }\
            }\
            var parent_element = document.getElementById(parent.id); \
            parent_element.submit();";
    }
    return code;

            /*
            //alert('Done setting text input');\
            //textInput[" + std::to_string(inputID) + "].onclick=function(){alert('Clicked!');};\
            //textInput[" + std::to_string(inputID) + "].onkeypress=function(){alert('Key pressed!');};\
            //textInput[" + std::to_string(inputID) + "].click();\
            //textInput[" + std::to_string(inputID) + "].keypress();\
            //alert('--- Done ---');";
            */
}

std::string jsFavIconUpdate(std::string oldUrl)
{
    return
    //alert('favIcon Update script');
   "for (i = 0; i < links.length; i++)\
    {\
        if(links[i].rel == 'icon' || links[i].rel == 'shortcut icon')\
        {\
            window.favIconUrl = links[i].href;\
        }\
    }"
    /*if(window.favIconUrl == '')\
        alert('(Update) Searched '+links.length+', have not found iconURL...');\*/
    "if(window.favIconUrl != " + oldUrl + ")\
        favIconImg.src = window.favIconUrl;\
    ";
}
