//============================================================================
// Distributed under the Apache License, Version 2.0.
//============================================================================

#pragma once

#include "src/Interface_Elements/Element.h"
#include "src/Buttons/WallButton.h"
#include "src/TwitterClient/TwitterClient.h"
#include <thread>

class WallContentArea : public Element {

public:

    WallContentArea(eyegui::Layout* pLayout);
    ~WallContentArea();
    void virtual show();
    void virtual hide();
    void selectTweet(std::string id);
    void showTweets();
    void updateNewsFeed(bool reset);
    void scrollUp(int i);
    void scrollDown(int i);
    std::string getTweetUser(int i);
    std::string getTweetIdAt(int i);
    int tweetIndex = 0;

private:

    bool getLikedAt(int i);
    bool getRetweetedAt(int i);
    bool getOwnAt(int i);
    std::string currentlySelected = "none";
    std::shared_ptr<WallButton> wallButtonListener = std::shared_ptr<WallButton>(new WallButton);
    rapidjson::Document content;
    int buttonFrames[4];
    int textFrames[4];
    std::string textboxes[12] = {	"textBlock1_1","textBlock1_2","textBlock1_3",
                                    "textBlock2_1","textBlock2_2","textBlock2_3",
                                    "textBlock3_1","textBlock3_2","textBlock3_3",
                                    "textBlock4_1","textBlock4_2","textBlock4_3"
                                };
};
