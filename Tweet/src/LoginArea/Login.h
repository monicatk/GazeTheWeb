//============================================================================
// Distributed under the Apache License, Version 2.0.
//============================================================================

#pragma once

#include "externals/eyeGUI-development/include/eyeGUI.h"
#include "src/LoginArea/LoginButton.h"
#include "src/LoginArea/CloseScreenButtons.h"
#include "src/TwitterApp.h"
#include <iostream>

class Login {

    public:

        static Login* createInstance(int width, int height);
        ~Login();
        void render();

        // Mandatory eyegui variables
        eyegui::GUI* pGUI;
        eyegui::Layout* pLayout;
		eyegui::Layout* close_layout;

        static Login* instance;

        void setID();
        void setPassword();
        void setApplication();

        std::string getID();
        std::string getPassword();
        TwitterApp* getApplication();

        void enterInformation();
        bool checkMatchIDPassword();
		void toCloseScreen();
		void toLoginScreen();

        //Keyboard Counter
        int CountR = 0;

        TwitterApp* application;
        std::string input;

        void enterText(std::string& input);

    private:

        Login(int width, int height);
        Login();

        // Variable to save the address of activeGUI
        eyegui::GUI** temp;


        // Id and password variables for login
        std::string id = ""; //Can be set for a default account
        std::string password = ""; //Can be set for a default account

        // Oxygen-Sans font
        std::string fontFile = "font/Oxygen-Sans.ttf";

        // Layout file for the Login-Screen
        std::string layoutFile = "login_layout.xeyegui";
		std::string close_layoutFile = "close_screen.xeyegui";

        // GUI-Settings
        int width;
        int height;
        eyegui::CharacterSet charSet = eyegui::CharacterSet::GERMANY_GERMAN;
        std::shared_ptr<LoginButton> LoginButtonListener = std::shared_ptr<LoginButton>(new LoginButton);
		std::shared_ptr<CloseScreenButtons> CloseScreenButtonListener = std::shared_ptr<CloseScreenButtons>(new CloseScreenButtons);
};
