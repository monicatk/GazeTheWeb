//============================================================================
// Distributed under the Apache License, Version 2.0.
//============================================================================

#include "TwitterApp.h"
#include <iostream>

/**
* Constructor for the TwitterApp Class
* this constructor is private, the TwitterApp class is only constructed through its public function "createInstance()"
* @param[in] width value for the TwitterApp Class
* @param[in] height value for the TwitterApp Class
*/
TwitterApp::TwitterApp(int width, int height) {
    this->width = width;
    this->height = height;

    eyegui::GUIBuilder guiBuilder;
    guiBuilder.width = width;
    guiBuilder.height = height;
    guiBuilder.characterSet = charSet;
    guiBuilder.fontFilepath = fontFile;
    pGUI = guiBuilder.construct();
    pLayout = eyegui::addLayout(pGUI, layoutFile);

    //-----------------------------------------------------------------------------------------------------
    //Keyboardtest Area
    pLayout2 = eyegui::addLayout(pGUI, layoutFile2);
    eyegui::setVisibilityOfLayout(pLayout2, false);
    keyboard = Keyboard::createInstance(pLayout, pLayout2);

    //-----------------------------------------------------------------------------------------------------

    // Instantiate various interface Elements
    menueButtonArea = new MenueBar(pLayout);
    menueButtonArea->show();
    actionButtonArea = new ActionBar(pLayout);
    wallContentArea = new WallContentArea(pLayout);
    profileContentArea = new ProfilePageArea(pLayout);
    connectPageArea = new ConnectPageArea(pLayout);
    discoverPageArea = new DiscoverPageArea(pLayout);
    searchPageArea = new SearchPageArea(pLayout);
}

/**
* startUpMethod function
* Old login funtion
*/
void TwitterApp::startUpMethod() {
    if (hasConnection()) {

        std::string tempname;
        if (userID[0] == '@')
        {
            tempname = userID.substr(1, userID.length());
        }
        else {
            tempname = userID;
        }
        rapidjson::Document temp = (TwitterApp::getInstance()->getTwitter()->showUser(tempname, false));
        userID = temp["id_str"].GetString();
        profileContentArea->setCurProfile(userID);
        changeState(WALL);
    }
}


//Singletons have to be set to 0, when they are not instanciated yet
TwitterApp* TwitterApp::instance = 0;

/**
* Disable function
* Disable the activity of an element in the interface
*/
void TwitterApp::Disable(std::string id) {
    eyegui::setElementActivity(pLayout, id, false);
}
/**
* createInstance function
* this function instanciates the TwitterApp-class and is public
* @param[out] TwitterApp is the instance of the TwitterApp
*/
TwitterApp* TwitterApp::createInstance(int width, int height) {

    // Check if an instance alrdy exists
    if (!instance)
    {
        // If there is no instance of TwitterApp yet, construct one
        std::cout << "TwitterApp Singleton created. Width: "+std::to_string(width)+" Height: "+ std::to_string(height) << std::endl;
        instance = new TwitterApp(width, height);
        return instance;
    }
    // If there is alrdy one give back this warning
    std::cout << "There cant be two instances of TwitterApp" << std::endl;
    return instance;
}

/**
* createInstance function
* returns the current  instance of the twitterApp, if it alrdy exists
* @param[out] TwitterApp is the instance of the TwitterApp
*/
TwitterApp* TwitterApp::getInstance() {
    if (!instance)
    {
        std::cout << "A instance doesnt exist yet, pls use createInstance(int, int)" << std::endl;
    }
    return instance;
}

/**
* hasConnection function
* checks if twitter has an connection == twitter is not null
* @param[out] bool
*/
bool TwitterApp::hasConnection() {
    return (twitter != null);
}

/**
* Enable function
* Enables the actity of an element in the interface
* @param[in] id is the id of the Element
*/
void TwitterApp::Enable(std::string id) {
    eyegui::setElementActivity(pLayout, id, true);
}

/**
* resetInterface function
* Resets all Areas
*/
void TwitterApp::resetInterface() {
    wallContentArea->hide();
    profileContentArea->hide();
    searchPageArea->hide();
    discoverPageArea->hide();
    connectPageArea->hide();
    actionButtonArea->changeToNone();
}

/**
* scrollUp function
* Scroll the shown conten up on the basis of an int
* @param[in] i is the int for how much will be scrolled
*/
void TwitterApp::scrollUp(int i) {
    switch (currentstate)
    {
    case WALL:
        wallContentArea->scrollUp(i);
        break;
    case CONNECT:
        break;
    case DISCOVER:
        break;
    case PROFILE:
        profileContentArea->scrollUp(i);
        break;
    case SEARCH:
        break;
    default:
        break;
    }
}

/**
* scrollDown function
* Scroll the shown conten down on the basis of an int
* @param[in] i is the int for how much will be scrolled
*/
void TwitterApp::scrollDown(int i) {
    switch (currentstate)
    {
    case WALL:
        wallContentArea->scrollDown(i);
        break;
    case CONNECT:
        break;
    case DISCOVER:
        break;
    case PROFILE:
        profileContentArea->scrollDown(i);
        break;
    case SEARCH:
        break;
    default:
        break;
    }
}

/**
* scrollUp function
* Scroll the shown conten up
*/
void TwitterApp::scrollUp() {
    switch (currentstate)
    {
    case WALL:
        wallContentArea->scrollUp(4);
        break;
    case CONNECT:
        break;
    case DISCOVER:
        break;
    case PROFILE:
        profileContentArea->scrollUp(3);
        break;
    case SEARCH:
        searchPageArea->scrollUp(4);
        break;
    default:
        break;
    }
}

/**
* scrollDown function
* Scroll the shown conten down
*/
void TwitterApp::scrollDown() {
    switch (currentstate)
    {
    case WALL:
        wallContentArea->scrollDown(4);
        break;
    case CONNECT:
        break;
    case DISCOVER:
        break;
    case PROFILE:
        profileContentArea->scrollDown(3);
        break;
    case SEARCH:
        searchPageArea->scrollDown(4);
        break;
    default:
        break;
    }
}

/**
* changeState function
* Changes state of the shown Content on the basis of an int
* @param[in] state
*/
void TwitterApp::changeState(int state) {
    switch (state)
    {
    case WALL:
        resetInterface();
        currentstate = state;
        menueButtonArea->disable(state);
        wallContentArea->show();
        actionButtonArea->show();
        wallContentArea->updateNewsFeed(true);
        break;
    case CONNECT:
        resetInterface();
        currentstate = state;
        menueButtonArea->disable(state);
        connectPageArea->show();
        actionButtonArea->show();
        break;
    case DISCOVER:
        resetInterface();
        currentstate = state;
        menueButtonArea->disable(state);
        discoverPageArea->show();
        actionButtonArea->show();
        discoverPageArea->updateDiscoveryChannel();
        break;
    case PROFILE:
        resetInterface();
        currentstate = state;
        menueButtonArea->disable(state);
        profileContentArea->show();
        actionButtonArea->show();
        profileContentArea->updateTimeline(false);
        break;
    case SEARCH:
        resetInterface();
        currentstate = state;
        menueButtonArea->disable(state);
        searchPageArea->show();
        actionButtonArea->show();
        break;
    default:
        break;
    }
}

/**
* toTheTop function
* Changes shown content to the first in index
*/
void TwitterApp::toTheTop() {
    actionButtonArea->changeToNone();
    switch (currentstate)
    {
    case WALL:
        wallContentArea->tweetIndex = 0;
        updateCurrentPage();
        break;
    case CONNECT:
        connectPageArea->index = 0;
        updateCurrentPage();
        break;
    case DISCOVER:

        break;
    case PROFILE:
        profileContentArea->tweetIndex = 0;
        updateCurrentPage();
        break;
    case SEARCH:
        searchPageArea->index = 0;
        updateCurrentPage();
        break;
    default:
        break;
    }
}

/**
* updateCurrentPage function
* Updates Conetent on the basis of current State
*/
void TwitterApp::updateCurrentPage() {
    actionButtonArea->changeToNone();
    switch (currentstate)
    {
    case WALL:
        wallContentArea->selectTweet("none");
        wallContentArea->updateNewsFeed(false);
        break;
    case CONNECT:
        connectPageArea->selectContent("none");
        connectPageArea->showStatus();
        break;
    case DISCOVER:
        discoverPageArea->selectTweet("none");
        discoverPageArea->updateDiscoveryChannel();
        break;
    case PROFILE:
        profileContentArea->selectContent("none");
        profileContentArea->updateTimeline(false);
        break;
    case SEARCH:
        searchPageArea->selectContent("none");
        searchPageArea->search();
        break;
    default:
        break;
    }
}

/**
* getGUI function
* Getter function for the current pGUI
* @param[out] GUI*
*/
eyegui::GUI* TwitterApp::getGUI() {
    return pGUI;
}

/**
* getGUI function
* renderfunction of Gui
*/
void TwitterApp::render() {
    eyegui::drawGUI(pGUI);
}

/**
* ~TwitterApp function
* terminates current Gui
*/
TwitterApp::~TwitterApp() {
    eyegui::terminateGUI(pGUI);
}
