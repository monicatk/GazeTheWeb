//============================================================================
// Distributed under the Apache License, Version 2.0.
//============================================================================

#include "Element.h"

/**
* Constructor for the Element Class
* @param[in] id of the element
* @param[in] brickfile, brick for the element used
* @param[in] pLayout current layout file of GUI
*/
Element::Element(std::string id, std::string brickfile, eyegui::Layout* layout) {
    this->id = id;
    this->brickFile = brickfile;
    this->pLayout = layout;
}

/**
* Constructor for the Element Class
* @param[in] id of the element
*/
Element::Element(std::string id) {
    this->id = id;
    brickFile = "";
}

/**
* instanciateAllActionButtons function
* Functions sets Element Activ or Inactive
* @param[in] state bool dicedes if Element is activ or nor
*/
void Element::enable(bool state) {
    eyegui::setElementActivity(pLayout, id, state);
}

/**
* hide function
* hides the instances of the Element
*/
void Element::hide() {
    if (active)
    {
        eyegui::replaceElementWithBlock(pLayout,id,false);
        active = false;
    }
}

/**
 * shows the element by replacing the current element with its id, by the brick, the new element contains
 */
void Element::show() {
    if (!active&&brickFile.compare(""))
    {
        active = true;
        eyegui::replaceElementWithBrick(pLayout, id, brickFile);
    }
}

/**
* ~Element function
* no content
*/
Element::~Element() {

}
