//============================================================================
// Distributed under the Apache License, Version 2.0.
//============================================================================

#pragma once

#include "src/Interface_Elements/Element.h"
#include "src/Buttons/MenueBarButton.h"
#include <iostream>

#define BACK_BUTTON "back_button"
#define WALL_BUTTON "wall_button"
#define CONNECT_BUTTON "connect_button"
#define DISCOVER_BUTTON "discover_button"
#define PROFILE_BUTTON "profile_button"
#define SEARCH_BUTTON "search_button"
#define TWEET_BUTTON "tweet_button"

class MenueBar :public Element {

public:

    MenueBar(eyegui::Layout* pLayout);
    void virtual show();
    ~MenueBar();
    std::shared_ptr<MenueBarButton> menueButtonListener = std::shared_ptr<MenueBarButton>(new MenueBarButton);
    void disable(int id);

private:
    std::string currentlyDisabled = WALL_BUTTON;
};
