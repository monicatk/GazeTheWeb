//============================================================================
// Distributed under the Apache License, Version 2.0.
//============================================================================

#include "ProfileButton.h"
#include "src/TwitterApp.h"

/**
* ProfileButton hit function
* When a Button is hit, different functions of the Class will be used
* @param[in] pLayout current layout file of GUI
* @param[in] id is the ID of the hit Button
*/
void ProfileButton::hit(eyegui::Layout* pLayout, std::string id)
{
    if (TwitterApp::getInstance()->currentstate == PROFILE) {
        TwitterApp::getInstance()->profileContentArea->selectContent(id);
    }

}

/**
* ProfileButton down function
* no content
* @param[in] pLayout current layout file of GUI
* @param[in] id is the ID of the hit Button
*/
void ProfileButton::down(eyegui::Layout* pLayout, std::string id) {
}

/**
* ProfileButton up function
* no content
* @param[in] pLayout current layout file of GUI
* @param[in] id is the ID of the hit Button
*/
void ProfileButton::up(eyegui::Layout* pLayout, std::string id) {
}

