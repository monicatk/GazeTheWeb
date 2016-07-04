//============================================================================
// Distributed under the Apache License, Version 2.0.
//============================================================================

#include "ActionBarButton.h"
#include "src/TwitterApp.h"
#include "src/Keyboard/Keyboard.h"

/**
* ActionBarButton hit function
* When a Button is hit, different functions of some Classes will be used
* @param[in] pLayout current layout file of GUI
* @param[in] id is the ID of the hit Button
*/
void ActionBarButton::hit(eyegui::Layout* pLayout, std::string id)
{
    if (TwitterApp::getInstance()->hasConnection()) {
        std::cout << id + " has been hit" << std::endl;


        if (id.compare("likeButton") == 0) {

            std::string tweetid = TwitterApp::getInstance()->actionButtonArea->tweetId;
            TwitterApp::getInstance()->getTwitter()->createFavorites(tweetid);
            TwitterApp::getInstance()->updateCurrentPage();
        }
        if (id.compare("writePNButton") == 0) {
                        Keyboard::getInstance()->activate();
            Keyboard::getInstance()->setCas(3);
            Keyboard::getInstance()->setPointer(keyWord);
        }

        if (id.compare("dislikeButton") == 0) {

            std::string tweetid = TwitterApp::getInstance()->actionButtonArea->tweetId;
            TwitterApp::getInstance()->getTwitter()->destroyFavorites(tweetid);
            TwitterApp::getInstance()->updateCurrentPage();
        }

        if (id.compare("goToProfileButton") == 0) {


            std::string userID = TwitterApp::getInstance()->actionButtonArea->userId;
            TwitterApp::getInstance()->profileContentArea->setCurProfile(userID);

            TwitterApp::getInstance()->changeState(PROFILE);

        }

        if (id.compare("showImageButton") == 0) {

            std::cout << "Image with " + TwitterApp::getInstance()->actionButtonArea->imageUrls +" is being displayed"<< std::endl;
            TwitterApp::getInstance()->actionButtonArea->openImageFrame();

        }

        if (id.compare("respondButton") == 0) {
            std::string tweetid = TwitterApp::getInstance()->actionButtonArea->tweetId;
            Keyboard::getInstance()->activate();
            Keyboard::getInstance()->setCas(2);
            Keyboard::getInstance()->setId(tweetid);
            TwitterApp::getInstance()->updateCurrentPage();


        }
    }

    if (id.compare("followButton") == 0) {
        TwitterApp::getInstance()->getTwitter()->createFriendship(TwitterApp::getInstance()->actionButtonArea->userId, true);
        TwitterApp::getInstance()->updateCurrentPage();

    }
    if (id.compare("searchForButton") == 0) {
        TwitterApp::getInstance()->searchPageArea->setKeyWord(TwitterApp::getInstance()->actionButtonArea->userId);
        if (TwitterApp::getInstance()->searchPageArea->userSearch)
        {
            TwitterApp::getInstance()->searchPageArea->switchSearch();
        }
        TwitterApp::getInstance()->changeState(SEARCH);
        TwitterApp::getInstance()->searchPageArea->search();

    }
    if (id.compare("unfollowButton") == 0) {
        TwitterApp::getInstance()->getTwitter()->destroyFriendship(TwitterApp::getInstance()->actionButtonArea->userId, true);
        TwitterApp::getInstance()->updateCurrentPage();

    }
    if (id.compare("retweetButton") == 0) {
        std::string tweetid = TwitterApp::getInstance()->actionButtonArea->tweetId;
        TwitterApp::getInstance()->getTwitter()->retweet(tweetid);
        TwitterApp::getInstance()->updateCurrentPage();
    }
    if (id.compare("deleteButton") == 0) {
        std::string tweetid = TwitterApp::getInstance()->actionButtonArea->tweetId;
        TwitterApp::getInstance()->getTwitter()->statusDestroy(tweetid);
        TwitterApp::getInstance()->updateCurrentPage();
    }
}

/**
* ActionBarButton down function
* Function not in use
* @param[in] pLayout current layout file of GUI
* @param[in] id is the ID of the hit Button
*/
void ActionBarButton::down(eyegui::Layout* pLayout, std::string id) {
}

/**
* ActionBarButto up function
* After a Button is hit and it goes up a different functions of some Classes will be used
* Here after the Keyboardclass was used the button goes up and a direct message will be posted
* @param[in] pLayout current layout file of GUI
* @param[in] id is the ID of the hit Button
*/
void ActionBarButton::up(eyegui::Layout* pLayout, std::string id) {
        if (id.compare("writePNButton") == 0) {
            TwitterApp::getInstance()->getTwitter()->sendDirectMessage(keyWord, TwitterApp::getInstance()->actionButtonArea->userId, true);
            keyWord="";
        }
}
