//============================================================================
// Distributed under the Apache License, Version 2.0.
//============================================================================

#include "SearchButton.h"
#include "src/TwitterApp.h"

/**
* SearchButton hit function
* When a Button is hit, different functions of the Class will be used
* @param[in] pLayout current layout file of GUI
* @param[in] id is the ID of the hit Button
*/
void SearchButton::hit(eyegui::Layout* pLayout, std::string id)
{
    if (TwitterApp::getInstance()->currentstate == SEARCH) {
        if (id.compare("searchKeyboard") == 0) {
            Keyboard::getInstance()->activate();
            Keyboard::getInstance()->setCas(3);
            Keyboard::getInstance()->setPointer(keyWord);
        }
        else if (id.compare("profileSearchButton") == 0) {
            TwitterApp::getInstance()->searchPageArea->switchSearch();
        }
        else if (id.compare("tweetSearchButton") == 0) {
            TwitterApp::getInstance()->searchPageArea->switchSearch();
        }
        else {
            TwitterApp::getInstance()->searchPageArea->selectContent(id);
        }
    }
}

/**
* SearchButton down function
* no content
* @param[in] pLayout current layout file of GUI
* @param[in] id is the ID of the hit Button
*/
void SearchButton::down(eyegui::Layout* pLayout, std::string id) {
}

/**
* SearchButton up function
* no content
* @param[in] pLayout current layout file of GUI
* @param[in] id is the ID of the hit Button
*/
void SearchButton::up(eyegui::Layout* pLayout, std::string id) {
    if (id.compare("searchKeyboard") == 0) {
        if (keyWord.compare(""))
        {
            TwitterApp::getInstance()->searchPageArea->setKeyWord(keyWord);
            TwitterApp::getInstance()->searchPageArea->search();
        }
    keyWord = "";
    }
}

