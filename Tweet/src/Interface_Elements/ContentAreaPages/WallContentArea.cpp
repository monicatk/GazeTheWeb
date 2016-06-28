//============================================================================
// Distributed under the Apache License, Version 2.0.
//============================================================================

#include "WallContentArea.h"
#include "src/TwitterApp.h"
#include "src/TwitterClient/ImageDownload.h"
#include <string>

/**
* Constructor for the WallContentArea Class
* @param[in] pLayout current layout file of GUI
* @param[in] Element a new Element created
*/
WallContentArea::WallContentArea(eyegui::Layout* pLayout) :Element("content_area", "bricks/WallContentArea/wall_Content_Brick.beyegui", pLayout) {
}

/**
* Show function
* lets the instance show up in the interface, also registers the necessary buttons listeners
*/
void WallContentArea::show() {
    if (!active)
    {
        Element::show();
        for (int i = 0; i <4; i++)
        {
            textFrames[i] = eyegui::addFloatingFrameWithBrick(pLayout, "bricks/WallContentArea/floatingTweetBlock" + std::to_string(i + 1) + ".beyegui", 0.178125f, 0.265f + (0.18f*i), 0.65f, 0.1675f);
            buttonFrames[i] = eyegui::addFloatingFrameWithBrick(pLayout, "bricks/WallContentArea/floatingButton" + std::to_string(i + 1) + ".beyegui", 0.178125f, 0.265f + (0.18f*i), 0.65f, 0.1675f);
            eyegui::registerButtonListener(pLayout, std::to_string(i), wallButtonListener);
        }
    }
}

/**
* hide function
* hides the instances of the ProfilePageArea and delets the floating frames
*/
void WallContentArea::hide() {
    if (active)
    {
        currentlySelected = "none";
        for (int i = 0; i < 4; i++)
        {
            eyegui::removeFloatingFrame(pLayout, textFrames[i]);
            eyegui::removeFloatingFrame(pLayout, buttonFrames[i]);
        }
    }
    Element::hide();
}

/**
* updateNewsFeed function
* updates timeline of user over Twitter
* @param[in] bool if the Tweetindex should be reseted to 0
*/
void WallContentArea::updateNewsFeed(bool reset) {

    if (TwitterApp::getInstance()->hasConnection()) {
        content = (TwitterApp::getInstance()->getTwitter()->getTimelineHome(""));


        if (content.Size() < 4) {
            std::cout << "Couldnt get 4 Tweets from twitter!" << std::endl;
        }
        if (reset)
        {
            tweetIndex = 0;
        }
        showTweets();
    }
}

/**
* scrollUp function
* Scroll the shown conten up on the basis of an int
* @param[in] i is the int for how much will be scrolled
*/
void WallContentArea::scrollUp(int i) {
    if (tweetIndex + 4+i <content.Size())
    {
        tweetIndex += i;
    }
    else {
        tweetIndex = content.Size() - 4;
    }
    showTweets();
}

/**
* scrollDown function
* Scroll the shown conten down on the basis of an int
* @param[in] i is the int for how much will be scrolled
*/
void WallContentArea::scrollDown(int i) {
    if (tweetIndex - i >0)
    {
        tweetIndex -= i;
    }
    else {
        tweetIndex = 0;
    }
    showTweets();
}

/**
* getTweetIdAt function
* Getter of the TweetId at a index
* @param[in] i int + index is the tweet for the id
* @param[out] string of the ID of the tweet
*/
std::string WallContentArea::getTweetIdAt(int i) {
    return content[i + tweetIndex]["id_str"].GetString();
}

/**
* getTweetUser function
* Getter of the TweetUser at a index
* @param[in] i int + index is the tweet for the id
* @param[out] string of the ID of the user
*/
std::string WallContentArea::getTweetUser(int i) {
    return content[i + tweetIndex]["user"]["id_str"].GetString();
}

/**
* getLikedAt function
* Getter of the bool if a tweet was favorited
* @param[in] i int + index is the tweet for the bool
* @param[out] bool if the the tweet was favorited
*/
bool WallContentArea::getLikedAt(int i) {
    return content[i + tweetIndex]["favorited"].GetBool();
}

/**
* getOwnAt function
* Getter of the bool if user is owner of selected tweet
* @param[in] i is the id of tweet
* @param[out] bool if the selected tweet is from the user
*/
bool WallContentArea::getOwnAt(int i) {
    if (getTweetUser(i).compare(TwitterApp::getInstance()->userID))
    {
        return false;
    }
    return true;
}
/**
* getRetweetedAt function
* Getter of the bool if a tweet was reTweeted
* @param[in] i int + index is the tweet for the bool
* @param[out] bool if the the tweet was retweeted
*/
bool WallContentArea::getRetweetedAt(int i) {
    return content[i + tweetIndex]["retweeted"].GetBool();
}

/**
* showTweets function
* shows Content of the SearchPageArea
* shows the the tweets wich where found
*/
void WallContentArea::showTweets() {
    std::string temp = "\n";
    for (rapidjson::SizeType i = tweetIndex; (i < content.Size()) && (i < tweetIndex + 4); i++) {
        eyegui::setContentOfTextBlock(pLayout, textboxes[3 * (i-tweetIndex)], content[i]["user"]["name"].GetString()+temp+content[i]["text"].GetString());
		if (content[i].HasMember("retweeted_status")) {
		eyegui::setContentOfTextBlock(pLayout, textboxes[3 * (i - tweetIndex) + 1], "Likes: " + std::to_string(content[i]["retweeted_status"]["favorite_count"].GetInt()));
		}
		else {
	    eyegui::setContentOfTextBlock(pLayout, textboxes[3 * (i - tweetIndex) + 1], "Likes: " + std::to_string(content[i]["favorite_count"].GetInt()));
		}
        eyegui::setContentOfTextBlock(pLayout, textboxes[3 * (i - tweetIndex) + 2], "Retweets: " + std::to_string(content[i]["retweet_count"].GetInt()));

        // Download avatar image:
        std::string str = content[i]["user"]["profile_image_url"].GetString();

        // Extract file extension
        size_t z = str.rfind('.', str.length());
        std::string ext = str.substr(z + 1, str.length() - z);

        // Set filename using this rule: profile_ + Profile ID + extension
        std::string filename = "img/tmp/profile_" + std::string(content[i]["user"]["id_str"].GetString()) + "." + ext;

        // Replace substring '_normal.' with '_400x400' to get profile image in the size of 400x400 pixels.
        int index = str.find("_normal.");
        str = str.replace(index, 8, "_400x400.");

        std::string path = CONTENT_PATH + std::string("/") + filename;
        download_image((char*)str.c_str(), (char*)path.c_str());

        eyegui::ImageAlignment alignment = eyegui::ImageAlignment::ORIGINAL;
        eyegui::replaceElementWithPicture(pLayout, "profilePic"+std::to_string(i - tweetIndex+1), filename, alignment, false);
    }
}

/**
* selectTweet function
* Selectes Tweet of the WallContentArea
* changes style of Elemten on hand of id
* functions of other classes used
* @param[in] id of the Selected tweet
*/
void WallContentArea::selectTweet(std::string id) {
        if (currentlySelected.compare(id))
        {
            if (currentlySelected.compare("none"))
            {
                eyegui::ImageAlignment alignment = eyegui::ImageAlignment::STRETCHED;
                eyegui::replaceElementWithPicture(pLayout, "tweet" + currentlySelected, "menuebar_elements/tweet_content2.png", alignment);
                eyegui::setStyleOfElement(pLayout, "tweet" + currentlySelected, "block");
                eyegui::replaceElementWithBlock(pLayout, "rightSide" + currentlySelected, false);
            }
            currentlySelected = id;
            if (id.compare("none"))
            {
                if (tweetIndex!=0&&stoi(currentlySelected)==0)
                {
                    scrollDown(1);
                    selectTweet("1");
                    return;
                }
                if (stoi(currentlySelected) == 3 && tweetIndex != content.Size() - 4)
                {
                    scrollUp(1);
                    selectTweet("2");
                    return;
                }

                eyegui::ImageAlignment alignment = eyegui::ImageAlignment::STRETCHED;
                eyegui::replaceElementWithPicture(pLayout, "tweet" + currentlySelected, "Eprojekt_Design/tweetSelected.png", alignment);
                eyegui::setStyleOfElement(pLayout, "tweet" + currentlySelected, "block");

                eyegui::replaceElementWithPicture(pLayout, "action_button_area", "actionbar_elements/WallActionBar/actionBarWall" + currentlySelected+".png", alignment);
                eyegui::setStyleOfElement(pLayout, "action_button_area", "block");


                eyegui::replaceElementWithPicture(pLayout, "rightSide"+currentlySelected, "Eprojekt_Design/tweetConnection.png", alignment);

                std::cout << "Tweet " + id + " has been hit" << std::endl;
                if (content[stoi(currentlySelected)+tweetIndex]["entities"].HasMember("media"))
                {
                    std::vector<std::string> links;
                    for (int i = 0; i < content[stoi(currentlySelected) + tweetIndex]["extended_entities"]["media"].Size(); i++)
                    {
                        links.push_back(content[stoi(currentlySelected) + tweetIndex]["extended_entities"]["media"][i]["media_url"].GetString());
                    }
                    TwitterApp::getInstance()->actionButtonArea->mediaLinks = links;
                    TwitterApp::getInstance()->actionButtonArea->changeToTweetsWithPics(getTweetIdAt(stoi(currentlySelected)), getTweetUser(stoi(currentlySelected)), getLikedAt(stoi(currentlySelected)), getRetweetedAt(stoi(currentlySelected)), getOwnAt(stoi(currentlySelected)));

                }
                else {
                    TwitterApp::getInstance()->actionButtonArea->changeToTweets(getTweetIdAt(stoi(currentlySelected)), getTweetUser(stoi(currentlySelected)), getLikedAt(stoi(currentlySelected)), getRetweetedAt(stoi(currentlySelected)), getOwnAt(stoi(currentlySelected)));

                }
                return;
            }
            TwitterApp::getInstance()->actionButtonArea->changeToNone();
        }

}

/**
* ~WallContentArea function
* no content
*/
WallContentArea::~WallContentArea() {

}
