//============================================================================
// Distributed under the Apache License, Version 2.0.
//============================================================================

#include "DiscoverButton.h"
#include "src/TwitterApp.h"

/**
* DiscoverButton hit function
* When a Button is hit, different functions of the Class will be used
* @param[in] pLayout current layout file of GUI
* @param[in] id is the ID of the hit Button
*/
void DiscoverButton::hit(eyegui::Layout* pLayout, std::string id)
{
    if (TwitterApp::getInstance()->currentstate == DISCOVER) {
        TwitterApp::getInstance()->discoverPageArea->selectTweet(id);
    }

}

/**
* DiscoverButton down function
* no content
* @param[in] pLayout current layout file of GUI
* @param[in] id is the ID of the hit Button
*/
void DiscoverButton::down(eyegui::Layout* pLayout, std::string id) {
}

/**
* DiscoverButton up function
* no content
* @param[in] pLayout current layout file of GUI
* @param[in] id is the ID of the hit Button
*/
void DiscoverButton::up(eyegui::Layout* pLayout, std::string id) {
}

