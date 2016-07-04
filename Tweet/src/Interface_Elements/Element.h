//============================================================================
// Distributed under the Apache License, Version 2.0.
//============================================================================

#pragma once

#include "externals/eyeGUI-development/include/eyeGUI.h"
#include <iostream>

//SuperClass for most interface-elements, saves its own id, aswell as its brickfile
class Element {

public:

    Element(std::string id, std::string brickfile, eyegui::Layout* layout);
    Element(std::string id);
    std::string getID() { return id; }
    std::string getBrick() { return brickFile; }
    eyegui::Layout* getLayout() { return pLayout; }
    void virtual enable(bool state);
    void virtual show();
    void virtual hide();
    ~Element();

protected:

    bool active = false;
    std::string id;
    std::string brickFile;
    eyegui::Layout* pLayout;
};
