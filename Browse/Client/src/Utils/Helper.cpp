//============================================================================
// Distributed under the Apache License, Version 2.0.
// Author: Raphael Menges (raphaelmenges@uni-koblenz.de)
//============================================================================

#include "Helper.h"
#include <sstream>
#include <iomanip>
#include <cmath>

std::string RGBAToHexString(glm::vec4 color)
{
    // Clamp color
    glm::clamp(color, glm::vec4(0,0,0,0), glm::vec4(1,1,1,1));

    // Get 8-Bits out of it
    unsigned int r = (unsigned int) (color.r * 255);
    unsigned int g = (unsigned int) (color.g * 255);
    unsigned int b = (unsigned int) (color.b * 255);
    unsigned int a = (unsigned int) (color.a * 255);

    // Create hexadecimal out of it
    unsigned int hexNumber = ((r & 0xff) << 24) + ((g & 0xff) << 16) + ((b & 0xff) << 8) + (a & 0xff);

    // Make string out of it
    std::stringstream ss;
    ss << std::showbase // show the 0x prefix
        << std::internal // fill between the prefix and the number
        << std::setfill('0'); // set filling
    ss << std::hex << std::setw(10) << hexNumber; // create 10 chars long string
    return ss.str();
}

std::string ShortenURL(std::string URL)
{
	// Get rid of http
	std::string delimiter = "http://";
	size_t pos = 0;
	if((pos = URL.find(delimiter)) != std::string::npos)
	{
		URL.erase(0, pos + delimiter.length());
	}

	// Get rid of https
	delimiter = "https://";
	pos = 0;
	if((pos = URL.find(delimiter)) != std::string::npos)
	{
		URL.erase(0, pos + delimiter.length());
	}

	// Get rid of www.
	delimiter = "www.";
	pos = 0;
	if((pos = URL.find(delimiter)) != std::string::npos)
	{
		URL.erase(0, pos + delimiter.length());
	}

	// Get rid of everything beyond and inclusive /
	delimiter = "/";
	pos = 0;
	std::string shortURL = URL;
	if((pos = URL.find(delimiter)) != std::string::npos)
	{
		return URL.substr(0, pos);
	}
	else
	{
		return URL;
	}
}

int MaximalMipMapLevel(int width, int height)
{
	return std::floor(std::log2(std::fmax((float)width, (float)height)));
}
