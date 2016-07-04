//============================================================================
// Distributed under the Apache License, Version 2.0.
//============================================================================

#include "Login.h"

/**
* Constructor for the Login Class
* @param[in] width value for the login Class
* @param[in] height value for the login Class
*/
Login::Login(int width, int height) {
    this->width = width;
    this->height = height;

    // TwitterApp
    application = TwitterApp::createInstance(1280, 800);
    pLayout = eyegui::addLayout(application->getInstance()->getGUI(), layoutFile);
    eyegui::setVisibilityOfLayout(application->getInstance()->pLayout, false);
	close_layout = eyegui::addLayout(application->getInstance()->getGUI(), close_layoutFile);
	eyegui::setVisibilityOfLayout(close_layout, false);

    // Bricks for buttons
    eyegui::replaceElementWithBrick(pLayout, "left", "login_bricks/left_button_area.beyegui", false);
    eyegui::replaceElementWithBrick(pLayout, "middle", "login_bricks/middle_button_area.beyegui", false);
    eyegui::replaceElementWithBrick(pLayout, "right", "login_bricks/right_button_area.beyegui", false);

    // Register button "connect_button"
    eyegui::registerButtonListener(pLayout, "connect_button", LoginButtonListener);

    // Register button "password_button"
    eyegui::registerButtonListener(pLayout, "password_button", LoginButtonListener);

    // Register button "username_button"
    eyegui::registerButtonListener(pLayout, "username_button", LoginButtonListener);

	//Register button "close_button"
	eyegui::registerButtonListener(pLayout, "close_button", LoginButtonListener);

	//Register buttons for close_screen
	//Register button "close_yes"
	eyegui::registerButtonListener(close_layout, "close_yes", CloseScreenButtonListener);

	//Register button "close_no"
	eyegui::registerButtonListener(close_layout, "close_no", CloseScreenButtonListener);
}

/**
* setID function
* sets ID (User) of the Login class
*/
void Login::setID()
{
    this->id = "@" + this->input;
}

/**
* setPassword function
* sets password of the Login class
*/
void Login::setPassword()
{
    this->password = this->input;
}

/**
* enterText function
* Opens the Keaboard instance to set the input value
* @param[in] input value reference for the keyboard functions
*/
void Login::enterText(std::string& input) {

    Keyboard::getInstance()->setPLayout(pLayout);
    Keyboard::getInstance()->activate();
    Keyboard::getInstance()->setCas(3);
    Keyboard::getInstance()->setPointer(input);
}

/**
* setApplication function
* starts TwitterApp if correctly logged in to a twitter account
* changes currently used Login layout to TwitterApp layout
*/
void Login::setApplication() {

    if (checkMatchIDPassword()) {
        eyegui::setVisibilityOfLayout(pLayout, false);
        eyegui::setVisibilityOfLayout(application->getInstance()->pLayout, true);
        TwitterApp::getInstance()->setUserID(getID());
        TwitterApp::getInstance()->startUpMethod();
        Keyboard::getInstance()->setPLayout(TwitterApp::getInstance()->pLayout);
    }
    else {
        eyegui::setVisibilityOfLayout(pLayout, true);
        eyegui::setVisibilityOfLayout(application->getInstance()->pLayout, false);
    }
}

/**
* getID function
* getter function for the id of Login Class
* @param[out] id current ID (User)
*/
std::string Login::getID()
{
    return this->id;
}
/**
* getPassword function
* getter function for the password of Login Class
* @param[out] password current password
*/
std::string Login::getPassword()
{
    return this->password;
}

/**
* getApplication function
* getter function for the application of Login Class
* @param[out] application current application
*/
TwitterApp* Login::getApplication() {

    return this->application;
}

/**
* enterInformation function
* Sets ID and password and invokes checkMatchIDPassword function. If the function returns false, "Wrong Username or Password" message appears on the screen
*/
void Login::enterInformation()
{
    setPassword();
    if(checkMatchIDPassword() == true){
    setID();
    }
    else {
        eyegui::setContentOfTextBlock(pLayout, "textblock", "Wrong Username or Password");
    }
}

/**
* checkMatchIDPassword function
* Checks if  id and password match. Returns true if they match to an existing twitter account
* @param[out] bool if check was positive
*/
bool Login::checkMatchIDPassword() {
        application->getInstance()->account.setTwitterUsername("");
        application->getInstance()->twitter = new Twitter(&(application->getInstance()->account), id, password, false);

        std::string checklogin;
        std::string nullstring;
        application->getInstance()->twitter->mpAccount->getLastWebResponse(checklogin);

        // Check if user credentials are valid.
        if (checklogin == "Error processing your OAuth request: Invalid oauth_verifier parameter" ||
            checklogin == "{\"errors\":[{\"code\":89,\"message\":\"Invalid or expired token.\"}]}" ||
            checklogin == nullstring) {
            application->getInstance()->twitter = application->getInstance()->null;

            return false;
        }

        // Check if applications rate limit is exceeded. If so, switch to different account.
        if (application->getInstance()->twitter->isLimit()) {
            application->getInstance()->twitter = application->getInstance()->null;
            std::cout << "\nLimits reached, retry: \n" << std::endl;
            application->getInstance()->twitter = new Twitter(&(application->getInstance()->account2), id, password, true);
        }

    return true;
}

/**
* toCloseScreen function
* swiches to "Close screen"
*/
void Login::toCloseScreen() {

	eyegui::setVisibilityOfLayout(close_layout, true);
	eyegui::setVisibilityOfLayout(pLayout, false);
}


/**
* toLoginScreen function
* swiches to "Login screen"
*/
void Login::toLoginScreen() {

	eyegui::setVisibilityOfLayout(close_layout, false);
	eyegui::setVisibilityOfLayout(pLayout, true);
}

//Singletons have to be set to 0, when they are not instanciated yet
Login* Login::instance = 0;

/**
* createInstance function
* creates an instant of the Login class
* @param[in] width value for the login Class
* @param[in] height value for the login Class
*/
Login* Login::createInstance(int width, int height)
{
    instance = new Login(width, height);
    return instance;
}

/**
* render function
* draws the gui of the Login class
*/
void Login::render() {
    eyegui::drawGUI(pGUI);
}

/**
* ~Login function
* changes application to an TwitterApp
*/
Login::~Login()
{
    application->~TwitterApp();
}
