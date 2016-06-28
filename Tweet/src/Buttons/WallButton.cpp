//============================================================================
// Distributed under the Apache License, Version 2.0.
//============================================================================

#include "WallButton.h"
#include "src/TwitterApp.h"

/**
* WallButton hit function
* When a Button is hit, different functions of the Class will be used
* @param[in] pLayout current layout file of GUI
* @param[in] id is the ID of the hit Button
*/
void WallButton::hit(eyegui::Layout* pLayout, std::string id)
{
    if (TwitterApp::getInstance()->currentstate==WALL) {
        TwitterApp::getInstance()->wallContentArea->selectTweet(id);
    }

}

/**
* WallButton down function
* no content
* @param[in] pLayout current layout file of GUI
* @param[in] id is the ID of the hit Button
*/
void WallButton::down(eyegui::Layout* pLayout, std::string id) {
}

/**
* WallButton up function
* no content
* @param[in] pLayout current layout file of GUI
* @param[in] id is the ID of the hit Button
*/
void WallButton::up(eyegui::Layout* pLayout, std::string id) {
}

