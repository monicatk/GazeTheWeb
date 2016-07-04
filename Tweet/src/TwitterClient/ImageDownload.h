//============================================================================
// Distributed under the Apache License, Version 2.0.
//============================================================================

// Image download is based on libcurl
#include "externals/twitCurl/twitcurl.h"

#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

/**
* Removes folder and containing files.
* @param[in] path
*/
void remove_folder(std::string path);

/**
* Downloads the image specified in 'url' to the location specified in 'outfilename'.
* @param[in] url
* @param[in] outfilename[FILENAME_MAX]
*/
void download_image(char* url, char outfilename[FILENAME_MAX]);
