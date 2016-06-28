//============================================================================
// Distributed under the Apache License, Version 2.0.
//============================================================================

#include "ProfilePageArea.h"
#include "src/TwitterApp.h"
#include "src/TwitterClient/ImageDownload.h"
#include <string>

/**
* Constructor for the ProfilePageArea Class
* constructor for the wall-contentarea
* @param[in] pLayout current layout file of GUI
* @param[in] Element a new Element created
*/
ProfilePageArea::ProfilePageArea(eyegui::Layout* pLayout) :Element("content_area", "bricks/ProfilePageArea/profile_Content_Brick.beyegui", pLayout) {
}

/**
* Show function
* lets the instance show up in the interface, also registers the necessary buttons listeners
*/
void ProfilePageArea::show() {
    if (!active)
    {
        textFrames[0] = eyegui::addFloatingFrameWithBrick(pLayout, "bricks/ProfilePageArea/floatingTweetBlock" + std::to_string( 1) + ".beyegui", 0.1765625f, 0.2575f , 0.6484375f, 0.24f);
        buttonFrames[0] = eyegui::addFloatingFrameWithBrick(pLayout, "bricks/ProfilePageArea/floatingButton" + std::to_string(1) + ".beyegui", 0.1765625f, 0.2575f, 0.6484375f, 0.24f);
        eyegui::registerButtonListener(pLayout, std::to_string(0), profileButtonListener);
        Element::show();
        for (int i = 1; i <4; i++)
        {
            textFrames[i] = eyegui::addFloatingFrameWithBrick(pLayout, "bricks/ProfilePageArea/floatingTweetBlock" + std::to_string(i + 1) + ".beyegui", 0.1765625f, 0.3575f + (0.155f*i), 0.6484375f, 0.1425f);
            buttonFrames[i] = eyegui::addFloatingFrameWithBrick(pLayout, "bricks/ProfilePageArea/floatingButton" + std::to_string(i + 1) + ".beyegui", 0.1765625f, 0.3575f + (0.155f*i), 0.6484375f, 0.1425f);
            eyegui::registerButtonListener(pLayout, std::to_string(i), profileButtonListener);
        }
        eyegui::replaceElementWithBrick(pLayout, "toTheTopButton", "bricks/ProfilePageArea/topNameBrick.beyegui");
    }
}

/**
* hide function
* hides the instances of the ProfilePageArea and delets the floating frames
*/
void ProfilePageArea::hide() {
    if (active)
    {
        currentlySelected = "none";
        for (int i = 0; i < 4; i++)
        {
            eyegui::removeFloatingFrame(pLayout, textFrames[i]);
            eyegui::removeFloatingFrame(pLayout, buttonFrames[i]);
        }
        eyegui::replaceElementWithBoxButton(pLayout, "toTheTopButton", "menuebar_elements/goTop2.png");
        eyegui::registerButtonListener(pLayout, "toTheTopButton", TwitterApp::getInstance()->menueButtonArea->menueButtonListener);
    }
    Element::hide();
}

/**
* setCurProfile function
* Setter for the currentProfile
* @param[in] profile is he new value
*/
void ProfilePageArea::setCurProfile(std::string profile) {
    currentProfile =profile;
}

/**
* getTweetId function
* Getter of the TweedId of the selected Tweet
* @param[out] string of the id
*/
std::string ProfilePageArea::getTweetId() {
    if (currentlySelected.compare("0"))
    {
        return tweetcontents[(stoi(currentlySelected) - 1) + tweetIndex]["id_str"].GetString();
    }
    return "";
}

/**
* getTweetId function
* Getter of bool if the selected tweed is favorited
* @param[out] bool if favorited
*/
bool ProfilePageArea::getTweetFavorited() {
    if (currentlySelected.compare("0"))
    {
        return tweetcontents[(stoi(currentlySelected) - 1) + tweetIndex]["favorited"].GetBool();
    }
    return false;
}

/**
* getTweetRetweeted function
* Getter of bool if the selected tweed was retweeted
* @param[out] bool if retweeted
*/
bool ProfilePageArea::getTweetRetweeted() {
    if (currentlySelected.compare("0"))
    {
        return tweetcontents[(stoi(currentlySelected) - 1) + tweetIndex]["retweeted"].GetBool();
    }
    return false;
}

/**
* getTweetOwned function
* Getter of bool if the selected tweed is owned by the user
* @param[out] bool if owned
*/
bool ProfilePageArea::getTweetOwned() {
    if (currentlySelected.compare("0"))
    {
        if (getUserId().compare(TwitterApp::getInstance()->userID))
        {
            return false;
        }
        return true;
    }
    return false;
}

/**
* getUserId function
* Getter of the UserId of the selected tweet
* @param[out] string of the id
*/
std::string ProfilePageArea::getUserId() {
    if (currentlySelected.compare("0"))
    {
        return tweetcontents[(stoi(currentlySelected) - 1) + tweetIndex]["user"]["id_str"].GetString();
    }
    return currentProfile;
}

/**
* selectContent function
* Selectes Element of the ProfilePageArea
* changes style of Elemten on hand of id
* functions of other classes used
* @param[in] id of the Selected Content
*/
void ProfilePageArea::selectContent(std::string id) {

    if (currentlySelected.compare(id))
    {
        if (currentlySelected.compare("none")&& currentlySelected.compare("0"))
        {
            eyegui::ImageAlignment alignment = eyegui::ImageAlignment::STRETCHED;
            eyegui::replaceElementWithPicture(pLayout, "content" + currentlySelected, "menuebar_elements/tweet_content2.png", alignment);
            eyegui::setStyleOfElement(pLayout, "content" + currentlySelected, "block");
            eyegui::replaceElementWithBlock(pLayout, "rightSide" + currentlySelected, false);
        }
        currentlySelected = id;
        if (id.compare("none"))
        {

            if (stoi(currentlySelected) == 1 && tweetIndex != 0)
            {
                scrollDown(1);
                selectContent("2");
                return;
            }
            if (stoi(currentlySelected) == 3 && tweetIndex != tweetcontents.Size() - 3)
            {
                scrollUp(1);
                selectContent("2");
                return;
            }

            eyegui::ImageAlignment alignment = eyegui::ImageAlignment::STRETCHED;

            eyegui::replaceElementWithPicture(pLayout, "action_button_area", "actionbar_elements/ProfileActionBar/actionBarProfile" + currentlySelected + ".png", alignment);
            eyegui::setStyleOfElement(pLayout, "action_button_area", "block");
            if (currentlySelected.compare("0"))
            {
                eyegui::replaceElementWithPicture(pLayout, "content" + currentlySelected, "Eprojekt_Design/tweetSelected.png", alignment);
                eyegui::setStyleOfElement(pLayout, "content" + currentlySelected, "block");
                eyegui::replaceElementWithPicture(pLayout, "rightSide" + currentlySelected, "Eprojekt_Design/tweetConnection.png", alignment);


            }
            std::cout << "Content " + id + " has been hit" << std::endl;
            if (id.compare("0"))
            {
                if (tweetcontents[stoi(currentlySelected) + tweetIndex - 1]["entities"].HasMember("media"))
                {
                    std::vector<std::string> links;
                    for (int i = 0; i < tweetcontents[stoi(currentlySelected)-1 + tweetIndex]["extended_entities"]["media"].Size(); i++)
                    {
                        links.push_back(tweetcontents[stoi(currentlySelected)-1 + tweetIndex]["extended_entities"]["media"][i]["media_url"].GetString());
                    }
                    TwitterApp::getInstance()->actionButtonArea->mediaLinks = links;
                    TwitterApp::getInstance()->actionButtonArea->changeToTweetsWithPics(getTweetId(), getUserId(), getTweetFavorited(), getTweetRetweeted(), getTweetOwned());

                }
                else {
                    TwitterApp::getInstance()->actionButtonArea->changeToTweets(getTweetId(), getUserId(), getTweetFavorited(), getTweetRetweeted(), getTweetOwned());

                }


                          }
            else {

                TwitterApp::getInstance()->actionButtonArea->changeToProfiles(getUserId(), usercontents["following"].GetBool());
            }
            return;
        }
        TwitterApp::getInstance()->actionButtonArea->changeToNone();
    }

}

/**
* updateTimeline function
* loads tweets from Twitter of the users timeline
* @param[in] reset if true timeline will be reset to 0 tweets
*/
void ProfilePageArea::updateTimeline(bool reset) {
    if (TwitterApp::getInstance()->hasConnection()) {
        tweetcontents = (TwitterApp::getInstance()->getTwitter()->getTimelineUser(true, true, 21, currentProfile, true));
        usercontents = (TwitterApp::getInstance()->getTwitter()->showUser(currentProfile, true));
        if (tweetcontents.Size() < 3) {
            std::cout << "Couldnt get 3 Tweets from twitter!" << std::endl;
        }
        if (reset)
        {
            tweetIndex = 0;
        }
        showCurrentProfile();
    }
}

/**
* showCurrentProfile function
* shows Content of the ProfilePageArea
* shows the users timeline of tweets
* shows the users Profile
*/
void ProfilePageArea::showCurrentProfile() {
    bool downloadImage = true ;
    if (downloadImage) {
        // Download avatar image:
        std::string str = usercontents["profile_image_url"].GetString();

        // Extract file extension
        size_t i = str.rfind('.', str.length());
        std::string ext = str.substr(i + 1, str.length() - i);

        // Set filename using this rule: profile_ + Profile ID + extension
        std::string filename = "img/tmp/profile_" + std::string(usercontents["id_str"].GetString()) + "." + ext;

        // Replace substring '_normal.' with '_400x400' to get profile image in the size of 400x400 pixels.
        int index = str.find("_normal.");
        str = str.replace(index, 8, "_400x400.");

        std::string path = CONTENT_PATH + std::string("/") + filename;
        download_image((char*)str.c_str(), (char*)path.c_str());

        eyegui::ImageAlignment alignment = eyegui::ImageAlignment::ORIGINAL;
        eyegui::replaceElementWithPicture(pLayout, "profilePic", filename, alignment, false);
    }
    std::string temp = " @";
    eyegui::setContentOfTextBlock(pLayout, "toTheTopButton", usercontents["name"].GetString() + temp + usercontents["screen_name"].GetString());

    eyegui::setContentOfTextBlock(pLayout, textboxes[1], to_string(usercontents["statuses_count"].GetInt()));
    eyegui::setContentOfTextBlock(pLayout, textboxes[2], std::to_string(usercontents["followers_count"].GetInt()));
    eyegui::setContentOfTextBlock(pLayout,"textBlock1_4", usercontents["description"].GetString());
    for (rapidjson::SizeType i = tweetIndex; (i <tweetcontents.Size()) && (i<tweetIndex + 3); i++) {
        eyegui::setContentOfTextBlock(pLayout, textboxes[3 * (i+1 - tweetIndex)], tweetcontents[i]["text"].GetString());
        eyegui::setContentOfTextBlock(pLayout, textboxes[3 * (i+1 - tweetIndex) + 1], "Likes: " + std::to_string(tweetcontents[i]["favorite_count"].GetInt()));
        eyegui::setContentOfTextBlock(pLayout, textboxes[3 * (i+1 - tweetIndex) + 2], "Retweets: " + std::to_string(tweetcontents[i]["retweet_count"].GetInt()));
    }
}

/**
* scrollUp function
* Scroll the shown conten up on the basis of an int
* @param[in] i is the int for how much will be scrolled
*/
void ProfilePageArea::scrollUp(int i) {
    if (tweetIndex + 3 + i <tweetcontents.Size())
    {
        tweetIndex += i;
    }
    else {
        tweetIndex = tweetcontents.Size() - 3;
    }
    showCurrentProfile();
}

/**
* scrollDown function
* Scroll the shown conten down on the basis of an int
* @param[in] i is the int for how much will be scrolled
*/
void ProfilePageArea::scrollDown(int i) {
    if (tweetIndex - i >0)
    {
        tweetIndex -= i;
    }
    else {
        tweetIndex = 0;
    }
    showCurrentProfile();
}

/**
* ~ProfilePageArea function
* no content
*/
ProfilePageArea::~ProfilePageArea() {

}
