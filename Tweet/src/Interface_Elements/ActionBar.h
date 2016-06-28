//============================================================================
// Distributed under the Apache License, Version 2.0.
//============================================================================

#pragma once

#include "src/Interface_Elements/Element.h"
#include "src/Buttons/ActionBarButton.h"
#include "src/Buttons/ImageFrameButton.h"
#include <iostream>
#include <vector>

#define NONE 0
#define TWEETS 1
#define PROFILES 2
#define HASHTAGS 3
#define MESSAGE 4

class ActionBar :public Element {

public:

    ActionBar(eyegui::Layout* pLayout);
    ~ActionBar();
    void changeToNone();
    void changeToTweets(std::string tweetid, std::string userid, bool liked, bool retweeted, bool own);
    void changeToTweetsWithPics(std::string tweetid, std::string userid, bool liked, bool retweeted, bool own);
    void changeToProfiles(std::string userid, bool followed);
    void changeToProfilesWithGo(std::string userid, bool followed);
    void changeToHashtags(std::string content);
    void changeToMessages(std::string userid);
    void openImageFrame();
    void closeImageFrame();
    std::string tweetId;
    std::string userId;
    std::string imageUrls;
    std::vector<std::string> mediaLinks;
    void scrollPics(int change);

private:

    void renewImage();
    int picIndex = 0;
    float buttonWidth = /*0.1f;*/ 0.074f;
    float buttonHeight = 0.11826f;
    float buttonX = /*0.858f;*/ 0.875f;
    float buttonY = /*0.2625f;*/ 0.28825f;
    float buttonGap = 0.13478f;
    int imageFrame;
    int imageBackground;
    bool imageFrameOpen=false;
    std::shared_ptr<ActionBarButton> actionButtonListener = std::shared_ptr<ActionBarButton>(new ActionBarButton);
    std::shared_ptr<ImageFrameButton> imageFrameButtonListener = std::shared_ptr<ImageFrameButton>(new ImageFrameButton);
    int likeButton, dislikeButton, retweetButton, followButton, respondButton, goToProfileButton, deleteButton, unfollowButton, searchForButton, showImageButton, writePNButton;
    int currentState = NONE;
    void makeButtonsInvisible();
    void instanciateAllActionButtons();
};
