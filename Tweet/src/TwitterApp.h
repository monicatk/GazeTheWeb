//============================================================================
// Distributed under the Apache License, Version 2.0.
//============================================================================

#pragma once

#include "src/Interface_Elements/Element.h"
#include "src/Interface_Elements/MenueBar.h"
#include "src/Interface_Elements/ActionBar.h"
#include "src/Interface_Elements/ContentAreaPages/WallContentArea.h"
#include "src/Interface_Elements/ContentAreaPages/ProfilePageArea.h"
#include "src/Interface_Elements/ContentAreaPages/SearchPageArea.h"
#include "src/Interface_Elements/ContentAreaPages/ConnectPageArea.h"
#include "src/Interface_Elements/ContentAreaPages/DiscoverPageArea.h"
#include "src/Keyboard/Keyboard.h"
#include "externals/eyeGUI-development/include/eyeGUI.h"

// Just some constants to avoid typing errors, also you dont have to lookup the names always
#define MENUE_BUTTON_AREA "menue_button_area"
#define CONTENT_AREA "content_area"
#define LEFT_SWITCH_BUTTON_AREA "left_switch_button_area"
#define RIGHT_SWITCH_BUTTON_AREA "right_switch_button_area"
#define ACTION_BUTTON_AREA "action_button_area"

#define NOTSET 0
#define WALL 1
#define DISCOVER 2
#define CONNECT 3
#define PROFILE 4
#define SEARCH 5

class TwitterApp {

public:

    static TwitterApp* createInstance(int width, int height);
    static TwitterApp* getInstance();
    void updateCurrentPage();
    ~TwitterApp();
    Twitter* getTwitter() { return twitter; }
    void render();
    eyegui::GUI* getGUI();
    void changeState(int state);
    void startUpMethod();
    void Disable(std::string id);
    void Enable(std::string id);
    bool hasConnection();
    void scrollUp(int i);
    void scrollDown(int i);
    void scrollUp();
    void scrollDown();
    void toTheTop();
    void setUserID(std::string userID) { this->userID = userID; }

    // GUI Elements
    MenueBar* menueButtonArea;
    ActionBar* actionButtonArea;
    WallContentArea* wallContentArea;
    SearchPageArea* searchPageArea;
    ConnectPageArea* connectPageArea;
    DiscoverPageArea* discoverPageArea;
    ProfilePageArea* profileContentArea;
    std::string userID;

    int currentstate = NOTSET;

    // Login Test
    eyegui::Layout* pLayout;
    Twitter* twitter;
    Twitter* null;
    twitCurl account;
    twitCurl account2;
    Keyboard* keyboard;
    bool terminate = false;

private:

    static TwitterApp* instance; // private cause of singleton-structure
    TwitterApp(int width, int height);
    TwitterApp();
    void resetInterface();

    // GUI-Settings
    int width;
    int height;
    eyegui::CharacterSet charSet = eyegui::CharacterSet::GERMANY_GERMAN;

    // Brick-files
    std::string fontFile = "font/Oxygen-Sans.ttf";
    std::string layoutFile = "layout_Wall.xeyegui";
    std::string layoutFile2 = "layout_Keyboard.xeyegui";

    eyegui::GUI* pGUI;
    //eyegui::Layout* pLayout;
    eyegui::Layout* pLayout2;
};
