//============================================================================
// Distributed under the Apache License, Version 2.0.
//============================================================================

#include "ActionBar.h"
#include "src/TwitterClient/ImageDownload.h"

/**
* Constructor for the ActionBar Class
* @param[in] pLayout current layout file of GUI
* @param[in] Element a new Element created
*/
ActionBar::ActionBar(eyegui::Layout* pLayout) :Element("action_button_area", "bricks/actionbuttonbrick.beyegui", pLayout) {
    instanciateAllActionButtons();
}

/**
* instanciateAllActionButtons function
* instanciates all ActionBar Bricks and register Buttons
*/
void ActionBar::instanciateAllActionButtons() {
    likeButton = eyegui::addFloatingFrameWithBrick(pLayout, "bricks/likeButton.beyegui", buttonX, buttonY, buttonWidth, buttonHeight);
    dislikeButton = eyegui::addFloatingFrameWithBrick(pLayout, "bricks/dislikeButton.beyegui", buttonX, buttonY, buttonWidth, buttonHeight);
    retweetButton = eyegui::addFloatingFrameWithBrick(pLayout, "bricks/retweetButton.beyegui", buttonX, buttonY + buttonGap, buttonWidth, buttonHeight);
    respondButton = eyegui::addFloatingFrameWithBrick(pLayout, "bricks/respondButton.beyegui", buttonX, buttonY + 2 * buttonGap, buttonWidth, buttonHeight);
    goToProfileButton = eyegui::addFloatingFrameWithBrick(pLayout, "bricks/goToProfileButton.beyegui", buttonX, buttonY + 3 * buttonGap, buttonWidth, buttonHeight);
    followButton = eyegui::addFloatingFrameWithBrick(pLayout, "bricks/followButton.beyegui", buttonX, buttonY + 4 * buttonGap, buttonWidth, buttonHeight);
    deleteButton = eyegui::addFloatingFrameWithBrick(pLayout, "bricks/deleteButton.beyegui", buttonX, buttonY + 5 * buttonGap, buttonWidth, buttonHeight);
    unfollowButton = eyegui::addFloatingFrameWithBrick(pLayout, "bricks/unfollowButton.beyegui", buttonX, buttonY + 6 * buttonGap, buttonWidth, buttonHeight);
    searchForButton = eyegui::addFloatingFrameWithBrick(pLayout, "bricks/searchForButton.beyegui", buttonX, buttonY + 6 * buttonGap, buttonWidth, buttonHeight);
    showImageButton = eyegui::addFloatingFrameWithBrick(pLayout, "bricks/showImageButton.beyegui", buttonX, buttonY + 6 * buttonGap, buttonWidth, buttonHeight);
    writePNButton = eyegui::addFloatingFrameWithBrick(pLayout, "bricks/writePNButton.beyegui", buttonX, buttonY + 6 * buttonGap, buttonWidth, buttonHeight);
    eyegui::registerButtonListener(pLayout, "likeButton", actionButtonListener);
    eyegui::registerButtonListener(pLayout, "dislikeButton", actionButtonListener);
    eyegui::registerButtonListener(pLayout, "followButton", actionButtonListener);
    eyegui::registerButtonListener(pLayout, "unfollowButton", actionButtonListener);
    eyegui::registerButtonListener(pLayout, "goToProfileButton", actionButtonListener);
    eyegui::registerButtonListener(pLayout, "retweetButton", actionButtonListener);
    eyegui::registerButtonListener(pLayout, "respondButton", actionButtonListener);
    eyegui::registerButtonListener(pLayout, "deleteButton", actionButtonListener);
    eyegui::registerButtonListener(pLayout, "searchForButton", actionButtonListener);
    eyegui::registerButtonListener(pLayout, "showImageButton", actionButtonListener);
    eyegui::registerButtonListener(pLayout, "writePNButton", actionButtonListener);
    makeButtonsInvisible();
}

/**
* makeButtonsInvisible function
* makes all Buttons visible
*/
void ActionBar::makeButtonsInvisible() {
    eyegui::setVisibilityOFloatingfFrame(pLayout, likeButton, false);
    eyegui::setVisibilityOFloatingfFrame(pLayout, retweetButton, false);
    eyegui::setVisibilityOFloatingfFrame(pLayout, respondButton, false);
    eyegui::setVisibilityOFloatingfFrame(pLayout, followButton, false);
    eyegui::setVisibilityOFloatingfFrame(pLayout, unfollowButton, false);
    eyegui::setVisibilityOFloatingfFrame(pLayout, goToProfileButton, false);
    eyegui::setVisibilityOFloatingfFrame(pLayout, deleteButton, false);
    eyegui::setVisibilityOFloatingfFrame(pLayout, dislikeButton, false);
    eyegui::setVisibilityOFloatingfFrame(pLayout, searchForButton, false);
    eyegui::setVisibilityOFloatingfFrame(pLayout, showImageButton, false);
    eyegui::setVisibilityOFloatingfFrame(pLayout, writePNButton, false);
}

/**
* changeToNone function
* resets tweetId and UserId
*/
void ActionBar::changeToNone() {
    tweetId = "";
    userId = "";
    makeButtonsInvisible();
    currentState = NONE;

    eyegui::ImageAlignment alignment = eyegui::ImageAlignment::STRETCHED;
    eyegui::replaceElementWithPicture(pLayout, "action_button_area", "actionbar_elements/actionbar_background2.png", alignment);
}

/**
* changeToMessages function
* changes to Message STATE of Class
* @param[in] userid is the new value of userId of  ActionBar class
*/
void ActionBar::changeToMessages(std::string userid) {

    tweetId = "";
    userId = userid;
    makeButtonsInvisible();
    currentState = MESSAGE;

    eyegui::setPositionOfFloatingFrame(pLayout, writePNButton, buttonX, buttonY);

    eyegui::setVisibilityOFloatingfFrame(pLayout, writePNButton, true);

    eyegui::setPositionOfFloatingFrame(pLayout, goToProfileButton, buttonX, buttonY+buttonGap);

    eyegui::setVisibilityOFloatingfFrame(pLayout, goToProfileButton, true);
}

/**
* changeToHashtags function
* changes to Message HASHTAGS of Class
* @param[in] content is the new value of userId of  ActionBar class
*/
void ActionBar::changeToHashtags(std::string content) {
    tweetId = "";
    userId = content;
    makeButtonsInvisible();
    currentState = HASHTAGS;

    eyegui::setPositionOfFloatingFrame(pLayout, searchForButton, buttonX, buttonY);

    eyegui::setVisibilityOFloatingfFrame(pLayout, searchForButton, true);
}

/**
* changeToProfiles function
* changes to PROFILES state of Class
* @param[in] userid is the new value of userId of  ActionBar class
* @param[in] followed desides which buttons will be visible
*/
void ActionBar::changeToProfiles(std::string userid, bool followed) {
    tweetId = "";
    userId = userid;
    makeButtonsInvisible();
    currentState = PROFILES;
    if (followed) {
        eyegui::setPositionOfFloatingFrame(pLayout, unfollowButton, buttonX, buttonY);
        eyegui::setVisibilityOFloatingfFrame(pLayout, unfollowButton, true);
        eyegui::setPositionOfFloatingFrame(pLayout, writePNButton, buttonX, buttonY + buttonGap);
        eyegui::setVisibilityOFloatingfFrame(pLayout, writePNButton, true);
    }
    else {
        eyegui::setPositionOfFloatingFrame(pLayout, followButton, buttonX, buttonY);
        eyegui::setVisibilityOFloatingfFrame(pLayout, followButton, true);
    }
}

/**
* changeToProfilesWithGo function
* changes to PROFILES state of Class with index
* @param[in] userid is the new value of userId of  ActionBar class
* @param[in] followed desides which buttons will be visible
*/
void ActionBar::changeToProfilesWithGo(std::string userid, bool followed) {
    tweetId = "";
    userId = userid;
    makeButtonsInvisible();
    currentState = PROFILES;
    int i = 0;
    if (followed) {
        eyegui::setPositionOfFloatingFrame(pLayout, unfollowButton, buttonX, buttonY);
        eyegui::setVisibilityOFloatingfFrame(pLayout, unfollowButton, true);
        eyegui::setPositionOfFloatingFrame(pLayout, writePNButton, buttonX, buttonY+buttonGap);
        eyegui::setVisibilityOFloatingfFrame(pLayout, writePNButton, true);
        i += 2;
    }
    else {
        eyegui::setPositionOfFloatingFrame(pLayout, followButton, buttonX, buttonY);
        eyegui::setVisibilityOFloatingfFrame(pLayout, followButton, true);
		i++;
    }

    eyegui::setPositionOfFloatingFrame(pLayout, goToProfileButton, buttonX, buttonY+buttonGap*i);
    eyegui::setVisibilityOFloatingfFrame(pLayout, goToProfileButton, true);
}

/**
* openImageFrame function
* opens Imageframe with Image
* scrolling left and right possible
*/
void ActionBar::openImageFrame() {
    closeImageFrame();
    imageBackground = eyegui::addFloatingFrameWithBrick(pLayout, "bricks/Utility/imageBackground.beyegui", 0.0f, 0.f, 1.f, 1.f);
    imageFrame = eyegui::addFloatingFrameWithBrick(pLayout, "bricks/Utility/actionbuttonbrick.beyegui", 0.0f, 0.f, 1.f, 1.f);
    eyegui::registerButtonListener(pLayout, "unshowImage", imageFrameButtonListener);
    eyegui::registerButtonListener(pLayout, "imageLeft", imageFrameButtonListener);
    eyegui::registerButtonListener(pLayout, "imageRight", imageFrameButtonListener);
    picIndex = 0;
    renewImage();
    imageFrameOpen = true;
}

/**
* renewImage function
* reloads image in Element
*/
void ActionBar::renewImage() {

    // Download avatar image:
    std::string str = mediaLinks.at(picIndex);

    // Extract file extension
    size_t i = str.rfind('.', str.length());
    std::string ext = str.substr(i + 1, str.length() - i);

    // Set filename using this rule: profile_ + Profile ID + extension
    std::string filename = "img/tmp/picture_" + str.substr(str.size() - 9, str.size() - 5) + "." + ext;

    std::string path = CONTENT_PATH + std::string("/") + filename;
    download_image((char*)str.c_str(), (char*)path.c_str());
    eyegui::ImageAlignment alignment = eyegui::ImageAlignment::ORIGINAL;
    eyegui::replaceElementWithPicture(pLayout, "shownPicture", filename, alignment, false);
}

/**
* scrollPics function
* Scroll the shown conten up or down on the basis of an int
* @param[in] change is the int for how much will be scrolled
*/
void ActionBar::scrollPics(int change) {
    picIndex += change;
    if (picIndex<0) {
        picIndex = 0;
    } else if (picIndex>=mediaLinks.size()) {
        picIndex = mediaLinks.size() - 1;
    }
    renewImage();
}

/**
* closeImageFrame function
* closes Imageframe with Image
*/
void ActionBar::closeImageFrame() {
    if (imageFrameOpen)
    {
        eyegui::removeFloatingFrame(pLayout, imageFrame);
        eyegui::removeFloatingFrame(pLayout, imageBackground);
        imageFrameOpen = false;
    }
}

/**
* changeToTweets function
* changes to TWEETS state of Class with index
* @param[in] tweetid is the new value of userId of  ActionBar class
* @param[in] userid is the new value of userId of  ActionBar class
* @param[in] liked desides if dislike or like buttons will be visible
* @param[in] retweeted desides if retweetButton buttons will be visible
* @param[in] own desides if retweetButton and deleteButton buttons will be visible
*/
void ActionBar::changeToTweets(std::string tweetid, std::string userid, bool liked, bool retweeted, bool own) {
    tweetId = tweetid;
    userId = userid;
    makeButtonsInvisible();
    currentState = TWEETS;
    int i = 0;

    if (liked)
    {
        eyegui::setPositionOfFloatingFrame(pLayout, dislikeButton, buttonX, buttonY + buttonGap*i);
        eyegui::setVisibilityOFloatingfFrame(pLayout, dislikeButton, true);
        i++;
    } else {
        eyegui::setPositionOfFloatingFrame(pLayout, likeButton, buttonX, buttonY + buttonGap*i);
        eyegui::setVisibilityOFloatingfFrame(pLayout, likeButton, true);
        i++;
    }

    if (!own&&!retweeted) {
        eyegui::setPositionOfFloatingFrame(pLayout, retweetButton, buttonX, buttonY + buttonGap*i);
        eyegui::setVisibilityOFloatingfFrame(pLayout, retweetButton, true);
        i++;
    }

    eyegui::setPositionOfFloatingFrame(pLayout, respondButton, buttonX, buttonY + buttonGap*i);
    eyegui::setVisibilityOFloatingfFrame(pLayout, respondButton, true);
    i++;
    eyegui::setPositionOfFloatingFrame(pLayout, goToProfileButton, buttonX, buttonY + buttonGap*i);
    eyegui::setVisibilityOFloatingfFrame(pLayout, goToProfileButton, true);
    i++;

    if (own)
    {
        eyegui::setPositionOfFloatingFrame(pLayout, deleteButton, buttonX, buttonY + buttonGap*i);
        eyegui::setVisibilityOFloatingfFrame(pLayout, deleteButton, true);
        i++;
    }

    return;
}

/**
* changeToTweetsWithPics function
* changes to TWEETS state of Class with index
* @param[in] tweetid is the new value of userId of  ActionBar class
* @param[in] userid is the new value of userId of  ActionBar class
* @param[in] liked desides if dislike or like buttons will be visible
* @param[in] retweeted desides if retweetButton buttons will be visible
* @param[in] own desides if retweetButton and deleteButton buttons will be visible
*/
void ActionBar::changeToTweetsWithPics(std::string tweetid, std::string userid, bool liked, bool retweeted, bool own) {
    imageUrls = " ";
    tweetId = tweetid;
    userId = userid;
    makeButtonsInvisible();
    currentState = TWEETS;
    int i = 0;

    if (liked)
    {
        eyegui::setPositionOfFloatingFrame(pLayout, dislikeButton, buttonX, buttonY + buttonGap*i);
        eyegui::setVisibilityOFloatingfFrame(pLayout, dislikeButton, true);
        i++;
    }
    else {
        eyegui::setPositionOfFloatingFrame(pLayout, likeButton, buttonX, buttonY + buttonGap*i);
        eyegui::setVisibilityOFloatingfFrame(pLayout, likeButton, true);
        i++;
    }

    if (!own&&!retweeted) {
        eyegui::setPositionOfFloatingFrame(pLayout, retweetButton, buttonX, buttonY + buttonGap*i);
        eyegui::setVisibilityOFloatingfFrame(pLayout, retweetButton, true);
        i++;
    }


    eyegui::setPositionOfFloatingFrame(pLayout, respondButton, buttonX, buttonY + buttonGap*i);
    eyegui::setVisibilityOFloatingfFrame(pLayout, respondButton, true);
    i++;
    eyegui::setPositionOfFloatingFrame(pLayout, goToProfileButton, buttonX, buttonY + buttonGap*i);
    eyegui::setVisibilityOFloatingfFrame(pLayout, goToProfileButton, true);
    i++;

    if (own)
    {
        eyegui::setPositionOfFloatingFrame(pLayout, deleteButton, buttonX, buttonY + buttonGap*i);
        eyegui::setVisibilityOFloatingfFrame(pLayout, deleteButton, true);
        i++;
    }

    eyegui::setPositionOfFloatingFrame(pLayout, showImageButton, buttonX, buttonY + buttonGap*i);
    eyegui::setVisibilityOFloatingfFrame(pLayout, showImageButton, true);

    return;
}

/**
* ~WallContentArea function
* no content
*/
ActionBar::~ActionBar() {

}
