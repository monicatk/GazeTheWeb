//============================================================================
// Distributed under the Apache License, Version 2.0.
//============================================================================

#pragma once

#include "src/Interface_Elements/Element.h"
#include "src/Buttons/DiscoverButton.h"
#include "src/Interface_Elements/ContentAreaPages/WallContentArea.h"

class DiscoverPageArea : public Element
{

public:

    DiscoverPageArea(eyegui::Layout* pLayout);
    ~DiscoverPageArea();
    void virtual show();
    void virtual hide();
    void selectTweet(std::string id);
    void showDiscoveryChannel();
    void updateDiscoveryChannel();

private:

    std::string currentlySelected = "none";
    rapidjson::Document content;
    std::shared_ptr<DiscoverButton> discoverButtonListener = std::shared_ptr<DiscoverButton>(new DiscoverButton);
    int buttonFrames[7];
    int suggestionnmbr = 0;
    int textFrames[7];
    std::string textboxes[10] = { "textBlock1_1",
        "textBlock2_1",
        "textBlock3_1",
        "textBlock4_1",
        "textBlock5_1","textBlock5_2",
        "textBlock6_1","textBlock6_2",
        "textBlock7_1","textBlock7_2"
    };
    rapidjson::Document suggestions;
    std::string backBoxes[7] = { "hashtag0",
        "hashtag1",
        "hashtag2",
        "hashtag3",
        "suggestion4",
        "suggestion5",
        "suggestion6"
    };
};
