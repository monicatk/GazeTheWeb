//============================================================================
// Distributed under the Apache License, Version 2.0.
//============================================================================

#include "DiscoverPageArea.h"
#include "src/TwitterApp.h"
#include "src/TwitterClient/ImageDownload.h"
#include <string>
#include <stdlib.h>
#include <time.h>

/**
* Constructor for the DiscoverPageArea Class
* @param[in] pLayout current layout file of GUI
* @param[in] Element a new Element created
*/
DiscoverPageArea::DiscoverPageArea(eyegui::Layout* pLayout) :Element("content_area", "bricks/DiscoverPageArea/discover_Content_Brick.beyegui", pLayout) {
    std::srand(time(NULL));
}

/**
* Show function
* lets the instance show up in the interface, also registers the necessary buttons listeners
*/
void DiscoverPageArea::show() {
    if (!active)
    {
        Element::show();
        for (int i = 0; i <4; i++)
        {
            textFrames[i] = eyegui::addFloatingFrameWithBrick(pLayout, "bricks/DiscoverPageArea/floatingTweetBlock" + std::to_string(i + 1) + ".beyegui", 0.18125f+(0.328125f*(i % 2)), 0.2675f + (0.105f*(i/2)), 0.321875f, 0.095f);
            buttonFrames[i] = eyegui::addFloatingFrameWithBrick(pLayout, "bricks/DiscoverPageArea/floatingButton" + std::to_string(i + 1) + ".beyegui", 0.18125f+(0.328125f*(i%2)), 0.2675f + (0.105f*(i/2)), 0.321875f, 0.095f);
            eyegui::registerButtonListener(pLayout, std::to_string(i), discoverButtonListener);
        }

        textFrames[4] = eyegui::addFloatingFrameWithBrick(pLayout, "bricks/DiscoverPageArea/floatingTweetBlock5.beyegui", 0.18125f,0.4875f, 0.65f, 0.14375f);
        buttonFrames[4] = eyegui::addFloatingFrameWithBrick(pLayout, "bricks/DiscoverPageArea/floatingButton5.beyegui", 0.18125f, 0.4875f, 0.65f, 0.14375f);
        eyegui::registerButtonListener(pLayout, std::to_string(4), discoverButtonListener);

        textFrames[5] = eyegui::addFloatingFrameWithBrick(pLayout, "bricks/DiscoverPageArea/floatingTweetBlock6.beyegui", 0.18125f,0.6525f, 0.65f, 0.14375f);
        buttonFrames[5] = eyegui::addFloatingFrameWithBrick(pLayout, "bricks/DiscoverPageArea/floatingButton6.beyegui", 0.18125f, 0.6525f, 0.65f, 0.14375f);
        eyegui::registerButtonListener(pLayout, std::to_string(5), discoverButtonListener);

        textFrames[6] = eyegui::addFloatingFrameWithBrick(pLayout, "bricks/DiscoverPageArea/floatingTweetBlock7.beyegui", 0.18125f, 0.8175f, 0.65f, 0.14375f);
        buttonFrames[6] = eyegui::addFloatingFrameWithBrick(pLayout, "bricks/DiscoverPageArea/floatingButton7.beyegui", 0.18125f, 0.8175f, 0.65f, 0.14375f);
        eyegui::registerButtonListener(pLayout, std::to_string(6), discoverButtonListener);
        eyegui::replaceElementWithBlock(pLayout, "toTheTopButton", true);
    }
}

/**
* hide function
* hides the instances of the DiscoverPageArea and delets the floating frames
*/
void DiscoverPageArea::hide() {
    if (active)
    {
        currentlySelected = "none";
        for (int i = 0; i < 7; i++)
        {
            eyegui::removeFloatingFrame(pLayout, textFrames[i]);
            eyegui::removeFloatingFrame(pLayout, buttonFrames[i]);
        }
        eyegui::replaceElementWithBoxButton(pLayout, "toTheTopButton", "menuebar_elements/goTop2.png", u"", "");
        eyegui::registerButtonListener(pLayout, "toTheTopButton", TwitterApp::getInstance()->menueButtonArea->menueButtonListener);
    }
    Element::hide();
}

/**
* updateDiscoveryChannel function
* loads new suggestions from Twitter and updates a rapidjson::Document
*/
void DiscoverPageArea::updateDiscoveryChannel() {

    if (TwitterApp::getInstance()->hasConnection()) {
    content = TwitterApp::getInstance()->getTwitter()->getCurrentTrends();

    rapidjson::Document jObj2 = TwitterApp::getInstance()->getTwitter()->suggestionsGetSlugs("de");

    int iSecret = rand() % jObj2.Size();

    suggestions = TwitterApp::getInstance()->getTwitter()->suggestionsGet(jObj2[iSecret]["slug"].GetString(), "de");

    showDiscoveryChannel();
    }
}

/**
* showDiscoveryChannel function
* loads Content of the DiscoverPageArea
* Content shown  is Hashtags and 2 User Profiles
*/
void DiscoverPageArea::showDiscoveryChannel() {
    for (rapidjson::SizeType i = 0; (i < content[0]["trends"].Size()) && (i < 4); i++) {
        eyegui::setContentOfTextBlock(pLayout, textboxes[i ], content[0]["trends"][i]["name"].GetString());
    }
    suggestionnmbr = rand() % (suggestions["users"].Size() - 3);
    eyegui::setContentOfTextBlock(pLayout, textboxes[4], suggestions["users"][suggestionnmbr]["name"].GetString());
    eyegui::setContentOfTextBlock(pLayout, textboxes[5], "Current Follower: "+std::to_string(suggestions["users"][suggestionnmbr]["followers_count"].GetInt()));
    eyegui::setContentOfTextBlock(pLayout, textboxes[6], suggestions["users"][suggestionnmbr + 1]["name"].GetString());
    eyegui::setContentOfTextBlock(pLayout, textboxes[7], "Current Follower: " + std::to_string(suggestions["users"][suggestionnmbr + 1]["followers_count"].GetInt()));
    eyegui::setContentOfTextBlock(pLayout, textboxes[8], suggestions["users"][suggestionnmbr + 2]["name"].GetString());
    eyegui::setContentOfTextBlock(pLayout, textboxes[9], "Current Follower: " + std::to_string(suggestions["users"][suggestionnmbr + 2]["followers_count"].GetInt()));
    bool downloadImage = true;

    if (downloadImage) {
        // Download avatar image:
        std::string str = suggestions["users"][suggestionnmbr]["profile_image_url"].GetString();

        // Extract file extension
        size_t i = str.rfind('.', str.length());
        std::string ext = str.substr(i + 1, str.length() - i);

        // Set filename using this rule: profile_ + Profile ID + extension
        std::string filename = "img/tmp/profile_" + std::string(suggestions["users"][suggestionnmbr]["id_str"].GetString()) + "." + ext;

        // Replace substring '_normal.' with '_400x400' to get profile image in the size of 400x400 pixels.
        int index = str.find("_normal.");
        str = str.replace(index, 8, "_400x400.");

        std::string path = CONTENT_PATH + std::string("/") + filename;
        download_image((char*)str.c_str(), (char*)path.c_str());

        eyegui::ImageAlignment alignment = eyegui::ImageAlignment::ORIGINAL;
        eyegui::replaceElementWithPicture(pLayout, "picture1", filename, alignment, false);

        // Download avatar image:
        str = suggestions["users"][suggestionnmbr+1]["profile_image_url"].GetString();

        // Extract file extension
        i = str.rfind('.', str.length());
        ext = str.substr(i + 1, str.length() - i);

        // Set filename using this rule: profile_ + Profile ID + extension
        filename = "img/tmp/profile_" + std::string(suggestions["users"][suggestionnmbr+1]["id_str"].GetString()) + "." + ext;

        // Replace substring '_normal.' with '_400x400' to get profile image in the size of 400x400 pixels.
        index = str.find("_normal.");
        str = str.replace(index, 8, "_400x400.");

        path = CONTENT_PATH + std::string("/") + filename;
        download_image((char*)str.c_str(), (char*)path.c_str());

        alignment = eyegui::ImageAlignment::ORIGINAL;
        eyegui::replaceElementWithPicture(pLayout, "picture2", filename, alignment, false);

        // Download avatar image:
        str = suggestions["users"][suggestionnmbr + 2]["profile_image_url"].GetString();

        // Extract file extension
        i = str.rfind('.', str.length());
        ext = str.substr(i + 1, str.length() - i);

        // Set filename using this rule: profile_ + Profile ID + extension
        filename = "img/tmp/profile_" + std::string(suggestions["users"][suggestionnmbr + 2]["id_str"].GetString()) + "." + ext;

        // Replace substring '_normal.' with '_400x400' to get profile image in the size of 400x400 pixels.
        index = str.find("_normal.");
        str = str.replace(index, 8, "_400x400.");

        path = CONTENT_PATH + std::string("/") + filename;
        download_image((char*)str.c_str(), (char*)path.c_str());

        alignment = eyegui::ImageAlignment::ORIGINAL;
        eyegui::replaceElementWithPicture(pLayout, "picture3", filename, alignment, false);
    }
}

/**
* selectTweet function
* changes style of Elemten on hand of id
* functions of other classes used
* @param[in] id is ID of selected Contend
*/
void DiscoverPageArea::selectTweet(std::string id) {
    if (currentlySelected.compare(id))
    {
        if (id.compare("none"))
        {
            if (currentlySelected.compare("none"))
            {
                if (stoi(currentlySelected)<4)
                {
                    eyegui::ImageAlignment alignment = eyegui::ImageAlignment::STRETCHED;
                    eyegui::replaceElementWithPicture(pLayout, "hashtag" + currentlySelected, "menuebar_elements/tweet_content3.png", alignment);
                    eyegui::setStyleOfElement(pLayout, "hashtag" + currentlySelected, "invisible");

                }
                else {
                    eyegui::ImageAlignment alignment = eyegui::ImageAlignment::STRETCHED;
                    eyegui::replaceElementWithPicture(pLayout, "suggestion" + currentlySelected, "menuebar_elements/tweet_content4.png", alignment);
                    eyegui::setStyleOfElement(pLayout, "suggestion" + currentlySelected, "invisible");
                    eyegui::replaceElementWithBlock(pLayout, "rightSide" + currentlySelected, false);

                }
            }
            currentlySelected = id;
            int selection = stoi(currentlySelected);

            eyegui::ImageAlignment alignment = eyegui::ImageAlignment::STRETCHED;

            if (selection<4)
            {
                eyegui::replaceElementWithPicture(pLayout, "action_button_area", "actionbar_elements/DiscoverActionBar/actionBarDiscover0.png", alignment);
                eyegui::setStyleOfElement(pLayout, "action_button_area", "invisible");
                eyegui::replaceElementWithPicture(pLayout, "hashtag" + currentlySelected, "Eprojekt_Design/selectedHashtag.png", alignment);
                eyegui::setStyleOfElement(pLayout, "hashtag" + currentlySelected, "invisible");

                TwitterApp::getInstance()->actionButtonArea->changeToHashtags(content[0]["trends"][selection]["name"].GetString());
                std::cout << "hashtag "+currentlySelected+" hit" << std::endl;

            }
            else {

                eyegui::replaceElementWithPicture(pLayout, "action_button_area", "actionbar_elements/DiscoverActionBar/actionBarDiscover" + currentlySelected + ".png", alignment);
                eyegui::setStyleOfElement(pLayout, "action_button_area", "invisible");
                eyegui::replaceElementWithPicture(pLayout, "suggestion" + currentlySelected, "Eprojekt_Design/selectedProfile.png", alignment);
                eyegui::setStyleOfElement(pLayout, "suggestion" + currentlySelected, "invisible");
                eyegui::replaceElementWithPicture(pLayout, "rightSide" + currentlySelected, "Eprojekt_Design/profileConnection.png", alignment);
                TwitterApp::getInstance()->actionButtonArea->changeToProfilesWithGo(suggestions["users"][(selection - 4+suggestionnmbr)]["id_str"].GetString(),suggestions["users"][(selection-4+ suggestionnmbr)]["following"].GetBool());
                std::cout << "profile " + currentlySelected + " hit" << std::endl;
            }
            return;
        }
        TwitterApp::getInstance()->actionButtonArea->changeToNone();
        currentlySelected = id;
    }

}

/**
* ~DiscoverPageArea function
* no content
*/
DiscoverPageArea::~DiscoverPageArea() {

}
