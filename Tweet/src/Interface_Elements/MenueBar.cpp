//============================================================================
// Distributed under the Apache License, Version 2.0.
//============================================================================

#include "MenueBar.h"

/**
* Constructor for the MenueBar Class
* @param[in] pLayout current layout file of GUI
* @param[in] Element a new Element created
*/
MenueBar::MenueBar(eyegui::Layout* pLayout):Element("menue_button_area", "bricks/menueBarBrick.beyegui", pLayout) {
}

/**
* show function
* lets the instance show up in the interface, also registers the necessary buttons listeners
*/
void MenueBar::show() {
    Element::show();
    eyegui::registerButtonListener(pLayout, BACK_BUTTON, menueButtonListener);
    eyegui::registerButtonListener(pLayout, WALL_BUTTON, menueButtonListener);
    eyegui::registerButtonListener(pLayout, CONNECT_BUTTON, menueButtonListener);
    eyegui::registerButtonListener(pLayout, DISCOVER_BUTTON, menueButtonListener);
    eyegui::registerButtonListener(pLayout, PROFILE_BUTTON, menueButtonListener);
    eyegui::registerButtonListener(pLayout, SEARCH_BUTTON, menueButtonListener);
    eyegui::registerButtonListener(pLayout, TWEET_BUTTON, menueButtonListener);
    eyegui::registerButtonListener(pLayout,"toTheTopButton" , menueButtonListener);
    eyegui::setElementActivity(pLayout, WALL_BUTTON, false);
}

/**
* disable function
* disables elements of the MenuBar on the basis of an int
* @param[in] id of the int for the case
*/
void MenueBar::disable(int id) {
    switch (id)
    {
    case 1:
        eyegui::setElementActivity(pLayout, currentlyDisabled, true, true);
        eyegui::setElementActivity(pLayout, WALL_BUTTON, false, true);
        currentlyDisabled = WALL_BUTTON;
        break;
    case 2:
        eyegui::setElementActivity(pLayout, currentlyDisabled, true, true);
        eyegui::setElementActivity(pLayout, DISCOVER_BUTTON, false, true);
        currentlyDisabled = DISCOVER_BUTTON;
        break;
    case 3:
        eyegui::setElementActivity(pLayout, currentlyDisabled, true, true);
        eyegui::setElementActivity(pLayout, CONNECT_BUTTON, false, true);
        currentlyDisabled = CONNECT_BUTTON;
        break;
    case 4:
        eyegui::setElementActivity(pLayout, currentlyDisabled, true, true);
        eyegui::setElementActivity(pLayout, PROFILE_BUTTON, false, true);
        currentlyDisabled = PROFILE_BUTTON;
        break;
    case 5:
        eyegui::setElementActivity(pLayout, currentlyDisabled, true, true);
        eyegui::setElementActivity(pLayout, SEARCH_BUTTON, false, true);
        currentlyDisabled = SEARCH_BUTTON;
        break;
    case 6:
        eyegui::setElementActivity(pLayout, currentlyDisabled, true, true);
        eyegui::setElementActivity(pLayout, TWEET_BUTTON, false, true);
        currentlyDisabled = TWEET_BUTTON;
        break;
    default:
        break;
    }
}

/**
* ~MenueBar function
* no content
*/
MenueBar::~MenueBar() {
    // TO-DO!!
}
