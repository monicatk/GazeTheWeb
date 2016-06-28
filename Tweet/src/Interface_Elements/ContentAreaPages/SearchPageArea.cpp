//============================================================================
// Distributed under the Apache License, Version 2.0.
//============================================================================

#include "SearchPageArea.h"
#include "src/TwitterApp.h"
#include "src/TwitterClient/ImageDownload.h"
#include <string>

/**
* Constructor for the SearchPageArea Class
* constructor for the wall-content area
* @param[in] pLayout current layout file of GUI
* @param[in] Element a new Element created
*/
SearchPageArea::SearchPageArea(eyegui::Layout* pLayout) :Element("content_area", "bricks/SearchPageArea/search_Content_Brick.beyegui", pLayout) {
}

/**
* Show function
* lets the instance show up in the interface, also registers the necessary buttons listeners
*/
void SearchPageArea::show() {
    if (!active)
    {

        Element::show();
        for (int i = 0; i <4; i++)
        {
            textFrames[i] = eyegui::addFloatingFrameWithBrick(pLayout, "bricks/SearchPageArea/floatingTweetBlock" + std::to_string(i + 1) + ".beyegui", 0.175f, 0.41f + 0.14125f*i, 0.65f, 0.1325f);
            buttonFrames[i] = eyegui::addFloatingFrameWithBrick(pLayout, "bricks/SearchPageArea/floatingButton" + std::to_string(i + 1) + ".beyegui", 0.175f, 0.41f + 0.14125f*i, 0.65f, 0.1325f);
            eyegui::registerButtonListener(pLayout, std::to_string(i), searchButtonListener);
        }
        textFrames[4] = eyegui::addFloatingFrameWithBrick(pLayout, "bricks/SearchPageArea/searchText.beyegui", 0.175f, 0.2875f, 0.65f, 0.11f);
        buttonFrames[4] = eyegui::addFloatingFrameWithBrick(pLayout, "bricks/SearchPageArea/searchButton.beyegui", 0.175f , 0.2875f , 0.65f, 0.11f);
        buttonFrames[5] = eyegui::addFloatingFrameWithBrick(pLayout, "bricks/SearchPageArea/tweetSearchButton.beyegui", 0.0f, 0.26f, 0.125f, 0.101f);
        buttonFrames[6] = eyegui::addFloatingFrameWithBrick(pLayout, "bricks/SearchPageArea/profileSearchButton.beyegui", 0.0f, 0.39f, 0.125f, 0.101f);
        eyegui::registerButtonListener(pLayout, "searchKeyboard", searchButtonListener);
        eyegui::registerButtonListener(pLayout, "tweetSearchButton", searchButtonListener);
        eyegui::registerButtonListener(pLayout, "profileSearchButton", searchButtonListener);
        eyegui::setContentOfTextBlock(pLayout, textboxes[0], searchWord);
        manageSearchButtons();
    }
    if (contentExists)
    {
        if (currentcontisuser)
        {
            showUsers();
        }
        else {
            showTweets();
        }
    }
}

/**
* selectContent function
* Selectes Element of the SearchPageArea
* changes style of Elemten on hand of id
* functions of other classes used
* @param[in] id of the Selected Content
*/
void SearchPageArea::selectContent(std::string id) {
    if (contentExists)	{

    if (currentlySelected.compare(id))
    {
        if (currentlySelected.compare("none"))
        {
            eyegui::ImageAlignment alignment = eyegui::ImageAlignment::STRETCHED;
            eyegui::replaceElementWithPicture(pLayout, "result" + currentlySelected, "menuebar_elements/tweet_content2.png", alignment);
            eyegui::setStyleOfElement(pLayout, "result" + currentlySelected, "block");
            eyegui::replaceElementWithBlock(pLayout, "rightSide" + currentlySelected, false);
        }
        currentlySelected = id;
        if (id.compare("none"))
        {

            if (index!=0&&stoi(currentlySelected)==0)
            {
                scrollDown(1);
                selectContent("1");
                return;
            }
            int size;
            if (currentcontisuser)
            {
                size = content.Size();
            }
            else {
                size = content["statuses"].Size();
            }
            if (stoi(currentlySelected) == 3 && index != size - 4)
            {
                scrollUp(1);
                selectContent("2");
                return;
            }
            eyegui::ImageAlignment alignment = eyegui::ImageAlignment::STRETCHED;
            eyegui::replaceElementWithPicture(pLayout, "result" + currentlySelected, "Eprojekt_Design/tweetSelected.png", alignment);
            eyegui::setStyleOfElement(pLayout, "result" + currentlySelected, "block");

            eyegui::replaceElementWithPicture(pLayout, "action_button_area", "actionbar_elements/SearchActionBar/actionBarSearch" + currentlySelected + ".png", alignment);
            eyegui::setStyleOfElement(pLayout, "action_button_area", "block");

            eyegui::replaceElementWithPicture(pLayout, "rightSide" + currentlySelected, "Eprojekt_Design/tweetConnection.png", alignment);

            std::cout << "Result " + id + " has been hit" << std::endl;
            if (stoi(currentlySelected)>=counter)
            {

                return;
            }
            if (currentcontisuser)
            {
                TwitterApp::getInstance()->actionButtonArea->changeToProfilesWithGo(content[index + stoi(currentlySelected)]["id_str"].GetString(), content[index + stoi(currentlySelected)]["following"].GetBool());
            }
            else {

                if (content["statuses"][stoi(currentlySelected) + index]["entities"].HasMember("media"))
                {
                    std::vector<std::string> links;
                    for (int i = 0; i < content["statuses"][stoi(currentlySelected) + index]["entities"]["media"].Size(); i++)
                    {
                        links.push_back(content["statuses"][stoi(currentlySelected) + index]["entities"]["media"][i]["media_url"].GetString());
                    }
                    TwitterApp::getInstance()->actionButtonArea->mediaLinks = links;
                    TwitterApp::getInstance()->actionButtonArea->changeToTweetsWithPics(getTweetIdAt( stoi(currentlySelected)), getTweetUser( stoi(currentlySelected)), getLikedAt( stoi(currentlySelected)), getRetweetedAt( stoi(currentlySelected)), getOwnAt( stoi(currentlySelected)));

                }
                else {
                    TwitterApp::getInstance()->actionButtonArea->changeToTweets(getTweetIdAt( stoi(currentlySelected)), getTweetUser( stoi(currentlySelected)), getLikedAt( stoi(currentlySelected)), getRetweetedAt(stoi(currentlySelected)), getOwnAt( stoi(currentlySelected)));
                }
            }
        return;
        }
        TwitterApp::getInstance()->actionButtonArea->changeToNone();
    }
    }

}

/**
* getTweetIdAt function
* Getter of the TweetId at a index
* @param[in] i int + index is the tweet for the id
* @param[out] string of the ID of the tweet
*/
std::string SearchPageArea::getTweetIdAt(int i) {
    return content["statuses"][i + index]["id_str"].GetString();
}

/**
* getTweetUser function
* Getter of the TweetUser at a index
* @param[in] i int + index is the tweet for the id
* @param[out] string of the ID of the user
*/
std::string SearchPageArea::getTweetUser(int i) {
    return content["statuses"][i + index]["user"]["id_str"].GetString();
}

/**
* getLikedAt function
* Getter of the bool if a tweet was favorited
* @param[in] i int + index is the tweet for the bool
* @param[out] bool if the the tweet was favorited
*/
bool SearchPageArea::getLikedAt(int i) {
    return content["statuses"][i + index]["favorited"].GetBool();
}

/**
* getOwnAt function
* Getter of the bool if user is owner of selected tweet
* @param[in] i is the id of tweet
* @param[out] bool if the selected tweet is from the user
*/
bool SearchPageArea::getOwnAt(int i) {
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
bool SearchPageArea::getRetweetedAt(int i) {
    return content["statuses"][i + index]["retweeted"].GetBool();
}

/**
* switchSearch function
* function changes the search function in the Gui
*/
void SearchPageArea::switchSearch() {
    userSearch = (!userSearch);
    manageSearchButtons();
}

/**
* setSearchPic function
* function changes the icon of Element of the searchSwitch
*/
void SearchPageArea::manageSearchButtons() {
    if (userSearch) {
        eyegui::setElementActivity(pLayout, "profileSearchButton", false);
        eyegui::setElementActivity(pLayout, "tweetSearchButton", true);
    }
    else {
        eyegui::setElementActivity(pLayout, "tweetSearchButton", false);
        eyegui::setElementActivity(pLayout, "profileSearchButton", true);
    }
}

/**
* scrollUp function
* Scroll the shown conten up on the basis of an int
* @param[in] i is the int for how much will be scrolled
*/
void SearchPageArea::scrollUp(int i) {
    selectContent("none");
    int size;
    if (currentcontisuser)
    {
        size = content.Size();
    }
    else {
        size = content["statuses"].Size();
    }
    if (index + 4 + i <size)
    {
        index += i;
    }
    else {
        index = size - 4;
    }
    if (currentcontisuser)
    {

        showUsers();
    }
    else {

        showTweets();
    }
}

/**
* scrollDown function
* Scroll the shown conten down on the basis of an int
* @param[in] i is the int for how much will be scrolled
*/
void SearchPageArea::scrollDown(int i) {
    selectContent("none");
    if (index - i >0)
    {
        index -= i;
    }
    else {
        index = 0;
    }
    if (currentcontisuser)
    {

        showUsers();
    }
    else {

        showTweets();
    }
}

/**
* search function
* functions searches after content in twitter with a searchWord
*/
void SearchPageArea::search() {
    selectContent("none");
    index = 0;
    if (searchWord=="") return;
    if (TwitterApp::getInstance()->hasConnection()) {
        if (userSearch)
        {
            content = TwitterApp::getInstance()->getTwitter()->userSearch(searchWord, "80");
            contentExists = true;
            currentcontisuser = true;
            showUsers();
        }
        else {
            content = TwitterApp::getInstance()->getTwitter()->search(searchWord, "80");
            contentExists = true;
            currentcontisuser = false;
            showTweets();
        }
    }
}

/**
* showTweets function
* shows Content of the SearchPageArea
* shows the the tweets wich where found
*/
void SearchPageArea::showTweets() {
    counter = 0;
    std::string temp = "\n";
    for (rapidjson::SizeType i = index; (i < content["statuses"].Size()) && (i < index + 4); i++) {
        counter++;
        eyegui::setContentOfTextBlock(pLayout, textboxes[3 * (i - index)+1], content["statuses"][i]["user"]["name"].GetString() + temp + content["statuses"][i]["text"].GetString());
        eyegui::setContentOfTextBlock(pLayout, textboxes[3 * (i - index) + 2], "Likes: " + std::to_string(content["statuses"][i]["favorite_count"].GetInt()));
        eyegui::setContentOfTextBlock(pLayout, textboxes[3 * (i - index) + 3], "Retweets: " + std::to_string(content["statuses"][i]["retweet_count"].GetInt()));

        // Download avatar image:
        std::string str = content["statuses"][i]["user"]["profile_image_url"].GetString();

            // Extract file extension
            size_t z = str.rfind('.', str.length());
        std::string ext = str.substr(z + 1, str.length() - z);

        // Set filename using this rule: profile_ + Profile ID + extension
        std::string filename = "img/tmp/profile_" + std::string(content["statuses"][i]["user"]["id_str"].GetString()) + "." + ext;

        // Replace substring '_normal.' with '_400x400' to get profile image in the size of 400x400 pixels.
        int indexStr = str.find("_normal.");
        str = str.replace(indexStr, 8, "_400x400.");

        std::string path = CONTENT_PATH + std::string("/") + filename;
        download_image((char*)str.c_str(), (char*)path.c_str());

        eyegui::ImageAlignment alignment = eyegui::ImageAlignment::ORIGINAL;
        eyegui::replaceElementWithPicture(pLayout, "profilePic" + std::to_string(i - index + 1), filename, alignment, false);


    }
    std::string fillUp = " ";
    for (int r = counter; r  < 4; r ++)
    {

        eyegui::setContentOfTextBlock(pLayout, textboxes[3 * (r) + 1], fillUp);
        eyegui::setContentOfTextBlock(pLayout, textboxes[3 * (r)+2], fillUp);
        eyegui::setContentOfTextBlock(pLayout, textboxes[3 * (r) + 3], fillUp);
        eyegui::ImageAlignment alignment = eyegui::ImageAlignment::ORIGINAL;
        eyegui::replaceElementWithPicture(pLayout, "profilePic" + std::to_string(r + 1), "img/invisProf.png", alignment, false);
    }
}

/**
* showUsers function
* shows Content of the SearchPageArea
* shows the the Users which where found
*/
void SearchPageArea::showUsers() {
    counter = 0;
    std::string temp = "\n";
    std::string temp2 = " @";
    for (rapidjson::SizeType i = index; (i < content.Size()) && (i < index + 4); i++) {
        counter++;
        eyegui::setContentOfTextBlock(pLayout, textboxes[3 * (i - index) + 2], "Follower: ");
        eyegui::setContentOfTextBlock(pLayout, textboxes[3 * (i - index) + 3], std::to_string(content[i]["followers_count"].GetInt()));
        eyegui::setContentOfTextBlock(pLayout, textboxes[3 * (i - index) + 1], content[i]["screen_name"].GetString() +temp2+ content[i]["name"].GetString()+temp+content[i]["description"].GetString());

        // Download avatar image:
        std::string str = content[i]["profile_image_url"].GetString();

        // Extract file extension
        size_t z = str.rfind('.', str.length());
        std::string ext = str.substr(z + 1, str.length() - z);

        // Set filename using this rule: profile_ + Profile ID + extension
        std::string filename = "img/tmp/profile_" + std::string(content[i]["id_str"].GetString()) + "." + ext;

        // Replace substring '_normal.' with '_400x400' to get profile image in the size of 400x400 pixels.
        int indexStr = str.find("_normal.");
        str = str.replace(indexStr, 8, "_400x400.");

        std::string path = CONTENT_PATH + std::string("/") + filename;
        download_image((char*)str.c_str(), (char*)path.c_str());

        eyegui::ImageAlignment alignment = eyegui::ImageAlignment::ORIGINAL;
        eyegui::replaceElementWithPicture(pLayout, "profilePic" + std::to_string(i - index + 1), filename, alignment, false);

    }
    std::string fillUp = " ";
    for (int r = counter; r < 4; r++)
    {

        eyegui::setContentOfTextBlock(pLayout, textboxes[3 * (r)+1], fillUp);
        eyegui::setContentOfTextBlock(pLayout, textboxes[3 * (r)+2], fillUp);
        eyegui::setContentOfTextBlock(pLayout, textboxes[3 * (r)+3], fillUp);
        eyegui::ImageAlignment alignment = eyegui::ImageAlignment::ORIGINAL;
        eyegui::replaceElementWithPicture(pLayout, "profilePic" + std::to_string(r + 1), "img/invisProf.png", alignment, false);
    }
}

/**
* setKeyWord function
* Setter of the searchWord
* Updates Textblock with new searchword
* @param[in] word is the new value of the searchword
*/
void SearchPageArea::setKeyWord(std::string word) {
    searchWord = word;
    eyegui::setContentOfTextBlock(pLayout, textboxes[0], searchWord);
}

/**
* hide function
* hides the instances of the ProfilePageArea and delets the floating frames
*/
void SearchPageArea::hide() {
    if (active)
    {
        currentlySelected = "none";
        for (int i = 0; i < 5; i++)
        {
            eyegui::removeFloatingFrame(pLayout, textFrames[i]);
            eyegui::removeFloatingFrame(pLayout, buttonFrames[i]);
        }
        eyegui::removeFloatingFrame(pLayout, buttonFrames[5]);
        eyegui::removeFloatingFrame(pLayout, buttonFrames[6]);
        index = 0;

    }
    Element::hide();
}

/**
* ~SearchPageArea function
* no content
*/
SearchPageArea::~SearchPageArea() {

}
