//============================================================================
// Distributed under the Apache License, Version 2.0.
//============================================================================

#include "SwitchButton.h"
#include "src/TwitterApp.h"

/**
* SwitchButton hit function
* When a Button is hit, different functions of the Class will be used
* @param[in] pLayout current layout file of GUI
* @param[in] id is the ID of the hit Button
*/
void SwitchButton::hit(eyegui::Layout* pLayout, std::string id)
{

    if (id.compare(LEFT_SWITCH_BUTTON_AREA) == 0) {
        TwitterApp::getInstance()->scrollDown();
    }

    if (id.compare(RIGHT_SWITCH_BUTTON_AREA) == 0) {
        TwitterApp::getInstance()->scrollUp();
    }

}

/**
* SwitchButton down function
* no content
* @param[in] pLayout current layout file of GUI
* @param[in] id is the ID of the hit Button
*/
void SwitchButton::down(eyegui::Layout* pLayout, std::string id) {
}

/**
* SwitchButton up function
* no content
* @param[in] pLayout current layout file of GUI
* @param[in] id is the ID of the hit Button
*/
void SwitchButton::up(eyegui::Layout* pLayout, std::string id) {
}

