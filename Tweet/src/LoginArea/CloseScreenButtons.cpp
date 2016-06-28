//============================================================================
// Distributed under the Apache License, Version 2.0.
//============================================================================

#include "CloseScreenButtons.h"
#include "src/LoginArea/Login.h"
#include <iostream>

/**
* CloseScreenButtons hit function
* When a Button is hit, different functions of the Class will be used
* (1) close_yes closes the application
* (2) close_no switches back to "Login screen"
* @param[in] close_layout current layout file of GUI
* @param[in] id is the ID of the hit Button
*/
void CloseScreenButtons::hit(eyegui::Layout * close_layout, std::string id)
{
	if (id.compare("close_yes") == 0) {
		Login::instance->getApplication()->getInstance()->terminate = true;
	}
	if (id.compare("close_no") == 0) {
		Login::instance->toLoginScreen();
	}
}

/**
* LoginButton up function
* no content
* @param[in] close_layout current layout file of GUI
* @param[in] id is the ID of the hit Button
*/
void CloseScreenButtons::up(eyegui::Layout * close_layout, std::string id)
{
}

/**
* LoginButton down function
* no content
* @param[in] close_layout current layout file of GUI
* @param[in] id is the ID of the hit Button
*/
void CloseScreenButtons::down(eyegui::Layout * close_layout, std::string id)
{
}