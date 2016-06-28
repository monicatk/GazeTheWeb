//============================================================================
// Distributed under the Apache License, Version 2.0.
//============================================================================

#pragma once

#include "src/Interface_Elements/Element.h"
#include "src/Interface_Elements/ContentAreaPages/WallContentArea.h"
#include "src/Buttons/SearchButton.h"

class SearchPageArea:public Element{

public:
    SearchPageArea(eyegui::Layout* pLayout);
    ~SearchPageArea();
    void virtual show();
    void virtual hide();
    void selectContent(std::string id);
    void switchSearch();
    void search();
    void scrollUp(int i);
    void scrollDown(int i);
    std::string getTweetUser(int i);
    std::string getTweetIdAt(int i);
    void setKeyWord(std::string word);
    bool userSearch = false;
    int index = 0;

private:

    bool getLikedAt(int i);
    bool getRetweetedAt(int i);
    bool getOwnAt(int i);
    int counter = 0;
    void showTweets();
    void showUsers();
    std::string searchWord = "look here to type!";
    bool contentExists = false;
    void manageSearchButtons();
    rapidjson::Document content;
    bool currentcontisuser = false;
    std::string currentlySelected = "none";
    std::string textboxes[13] = { "searchText",
        "textBlock1_1","textBlock1_2","textBlock1_3",
        "textBlock2_1","textBlock2_2","textBlock2_3",
        "textBlock3_1","textBlock3_2","textBlock3_3",
        "textBlock4_1","textBlock4_2","textBlock4_3",
    };
    std::shared_ptr<SearchButton> searchButtonListener = std::shared_ptr<SearchButton>(new SearchButton);
    int buttonFrames[7];
    int textFrames[5];
};
