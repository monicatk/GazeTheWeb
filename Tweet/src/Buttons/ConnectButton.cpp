//============================================================================
// Distributed under the Apache License, Version 2.0.
//============================================================================

#include "WallButton.h"
#include "src/TwitterApp.h"

/**
* Connect button hit function
* When a Button is hit, different functions of the Class will be used
* @param[in] pLayout current layout file of GUI
* @param[in] id is the ID of the hit Button
*/
void ConnectButton::hit(eyegui::Layout* pLayout, std::string id)
{

    if (TwitterApp::getInstance()->currentstate == CONNECT) {
        if ((id.compare("receivedButton") == 0)|| (id.compare("sentButton") == 0)) {
            TwitterApp::getInstance()->connectPageArea->switchStatus();
        }
        else {
            TwitterApp::getInstance()->connectPageArea->selectContent(id);
        }
    }
}

/**
* Connect button down function
* no content
* @param[in] pLayout current layout file of GUI
* @param[in] id is the ID of the hit Button
*/
void ConnectButton::down(eyegui::Layout* pLayout, std::string id) {
}

/**
* Connect button up function
* no content
* @param[in] pLayout current layout file of GUI
* @param[in] id is the ID of the hit Button
*/
void ConnectButton::up(eyegui::Layout* pLayout, std::string id) {
}

