//============================================================================
// Distributed under the Apache License, Version 2.0.
//============================================================================

#include "ImageDownload.h"

/**
* Removes folder and containing files.
* @param[in] path
*/
void remove_folder(std::string path) {
    fs::remove_all(path);
}

/**
* Returns directory path from a complete file path
* @param[in] str
*/
std::string extract_file_path(const std::string& str)
{
    size_t found;
    found = str.find_last_of("/\\");
    //std::cout << " folder: " << str.substr(0, found) << std::endl;
    //std::cout << " file: " << str.substr(found + 1) << std::endl;
    return str.substr(0, found);
}

/**
* write_data function
* @param[in] *ptr
* @param[in] size
* @param[in] nmemb
* @param[in] *stream
* @param[out] size_t
*/
size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    size_t written;
    written = fwrite(ptr, size, nmemb, stream);
    return written;
}

/**
* download_image function
* @param[in] url
* @param[in] outfilename[FILENAME_MAX]
*/
void download_image(char* url, char outfilename[FILENAME_MAX]) {
    // If folder does not exist, create it
    if (!fs::exists(outfilename)) {
        std::string str(outfilename);
        str = extract_file_path(str);
        fs::create_directories(str);
    }

    CURL *curl;
    FILE *fp;
    CURLcode res;
    curl = curl_easy_init();
    if (curl)
    {
        fp = fopen(outfilename, "wb");
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        fclose(fp);
    }
}
