//============================================================================
// Distributed under the Apache License, Version 2.0.
//============================================================================

#pragma once

#include "src/Interface_Elements/Element.h"
#include "src/Interface_Elements/ContentAreaPages/WallContentArea.h"
#include "src/Buttons/ProfileButton.h"

class ProfilePageArea:public Element{

public:

    ProfilePageArea(eyegui::Layout* pLayout);
    ~ProfilePageArea();
    void virtual show();
    void virtual hide();
    void updateTimeline(bool reset);
    void showCurrentProfile();
    void setCurProfile(std::string profile);
    void selectContent(std::string id);
    void scrollUp(int i);
    void scrollDown(int i);
    int tweetIndex = 0;

private:

    std::string getTweetId();
    std::string getUserId();
    std::string currentlySelected = "none";
    bool getTweetFavorited();
    bool getTweetRetweeted();
    bool getTweetOwned();
    std::shared_ptr<ProfileButton> profileButtonListener = std::shared_ptr<ProfileButton>(new ProfileButton);
    rapidjson::Document tweetcontents;
    rapidjson::Document usercontents;
    std::string currentProfile;
    int buttonFrames[4];
    int textFrames[4];
    std::string textboxes[12] = { "textBlock1_1","textBlock1_2","textBlock1_3",
        "textBlock2_1","textBlock2_2","textBlock2_3",
        "textBlock3_1","textBlock3_2","textBlock3_3",
        "textBlock4_1","textBlock4_2","textBlock4_3"
    };
};
