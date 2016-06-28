//============================================================================
// Distributed under the Apache License, Version 2.0.
//============================================================================

#include "MenueBarButton.h"
#include "src/TwitterApp.h"
#include "src/Keyboard/Keyboard.h"

/**
* MenueBarButton hit function
* When a Button is hit, different functions of the Class will be used
* the functionality for the menuebarbuttons is implemented here
* @param[in] pLayout current layout file of GUI
* @param[in] id is the ID of the hit Button
*/
void MenueBarButton::hit(eyegui::Layout* pLayout, std::string id)
{
    //functionality for the Back Button
    if (id.compare("back_button") == 0) {
        std::cout << "Back-Button hit" << std::endl;
        TwitterApp::getInstance()->terminate = true;
    }


        //functionality for the Wall Button
        if (id.compare("toTheTopButton") == 0) {
            std::cout << "toTheTopButton hit" << std::endl;
            TwitterApp::getInstance()->toTheTop();
        }

    //functionality for the Wall Button
    if (id.compare("wall_button") == 0) {
        std::cout << "Wall-Button hit" << std::endl;
        TwitterApp::getInstance()->changeState(WALL);
    }

    //functionality for the Connect Button
    if (id.compare("connect_button") == 0) {
        std::cout << "Connect-Button hit" << std::endl;
        TwitterApp::getInstance()->changeState(CONNECT);
    }

    //functionality for the Discover Button
    if (id.compare("discover_button") == 0) {
        std::cout << "Discover-Button hit" << std::endl;
        TwitterApp::getInstance()->changeState(DISCOVER);
    }

    //functionality for the Profile Button
    if (id.compare("profile_button") == 0) {
        std::cout << "Profile-Button hit" << std::endl;
        TwitterApp::getInstance()->profileContentArea->setCurProfile(TwitterApp::getInstance()->userID);
        TwitterApp::getInstance()->changeState(PROFILE);
    }

    //functionality for the Search Button
    if (id.compare("search_button") == 0) {
        std::cout << "Search-Button hit" << std::endl;
        TwitterApp::getInstance()->changeState(SEARCH);
    }

    //functionality for the Tweet Button
    if (id.compare("tweet_button") == 0) {
        Keyboard::getInstance()->activate();
        Keyboard::getInstance()->setCas(1);
    }
}

/**
* MenueBarButton down function
* no content
* @param[in] pLayout current layout file of GUI
* @param[in] id is the ID of the hit Button
*/
void MenueBarButton::down(eyegui::Layout* pLayout, std::string id) {
}

/**
* MenueBarButton up function
* no content
* @param[in] pLayout current layout file of GUI
* @param[in] id is the ID of the hit Button
*/
void MenueBarButton::up(eyegui::Layout* pLayout, std::string id) {
}
