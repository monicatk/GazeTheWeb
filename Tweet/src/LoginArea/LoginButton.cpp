//============================================================================
// Distributed under the Apache License, Version 2.0.
//============================================================================

#include "LoginButton.h"
#include "src/LoginArea/Login.h"
#include <iostream>

/**
* LoginButton hit function
* When a Button is hit, different functions of the Class will be used
* (1) username_button enters ID (User)
* (2) password_button enters password
* (3) connect_button moves to the twitterApp screen, if the used ID and password match
* (4) close_button closes the application
* @param[in] pLayout current layout file of GUI
* @param[in] id is the ID of the hit Button
*/
void LoginButton::hit(eyegui::Layout * pLayout, std::string id)
{
    if (id.compare("username_button") == 0) {
        Login::instance->enterText(Login::instance->input);
    }
    if (id.compare("password_button") == 0) {
        Login::instance->enterText(Login::instance->input);
    }
    if (id.compare("connect_button") == 0) {
        Login::instance->setApplication();
    }
	if (id.compare("close_button") == 0) {
		Login::instance->toCloseScreen();
		//Login::instance->getApplication()->getInstance()->terminate = true;
	}
}

/**
* LoginButton up function
* Extra functionality if a button is hit
* (1) username_button updates the entered username and shows it in a textbox on the screen
* (2) password_button updates the entered password
* (3) connect_button updates the layout with the warning "Wrong Username or Password" if ID and password don`t match
* @param[in] pLayout current layout file of GUI
* @param[in] id is the ID of the hit Button
*/
void LoginButton::up(eyegui::Layout * pLayout, std::string id)
{
    if (id.compare("username_button") == 0) {
        Login::instance->setID();
        eyegui::setContentOfTextBlock(Login::instance->pLayout, "textblock", Login::instance->getID());
    }
    if (id.compare("password_button") == 0) {

        Login::instance->setPassword();
    }
    if (id.compare("connect_button") == 0) {
        eyegui::setContentOfTextBlock(Login::instance->pLayout, "textblock", "Wrong Username or Password");
    }
}

/**
* LoginButton down function
* no content
* @param[in] pLayout current layout file of GUI
* @param[in] id is the ID of the hit Button
*/
void LoginButton::down(eyegui::Layout * pLayout, std::string id)
{
}
