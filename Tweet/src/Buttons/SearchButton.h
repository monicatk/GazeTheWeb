//============================================================================
// Distributed under the Apache License, Version 2.0.
//============================================================================

#pragma once

#include "externals/eyeGUI-development/include/eyeGUI.h"
#include <iostream>

class SearchButton :public eyegui::ButtonListener {

public:

    std::string keyWord="";
    void virtual hit(eyegui::Layout* pLayout, std::string id);
    void virtual up(eyegui::Layout* pLayout, std::string id);
    void virtual down(eyegui::Layout* pLayout, std::string id);
};
