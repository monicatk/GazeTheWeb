//============================================================================
// Distributed under the Apache License, Version 2.0.
//============================================================================

#include "ImageFrameButton.h"
#include "src/TwitterApp.h"

/**
* ImageFrameButton hit function
* When a Button is hit, different functions of the Class will be used
* @param[in] pLayout current layout file of GUI
* @param[in] id is the ID of the hit Button
*/
void ImageFrameButton::hit(eyegui::Layout* pLayout, std::string id)
{
    if (id.compare("unshowImage") == 0) {
        TwitterApp::getInstance()->actionButtonArea->closeImageFrame();
    }
    if (id.compare("imageLeft") == 0) {
        TwitterApp::getInstance()->actionButtonArea->scrollPics(-1);
    }
    if (id.compare("imageRight") == 0) {
        TwitterApp::getInstance()->actionButtonArea->scrollPics(1);
    }
}

/**
* ImageFrameButton down function
* no content
* @param[in] pLayout current layout file of GUI
* @param[in] id is the ID of the hit Button
*/
void ImageFrameButton::down(eyegui::Layout* pLayout, std::string id) {
}

/**
* ImageFrameButton up function
* no content
* @param[in] pLayout current layout file of GUI
* @param[in] id is the ID of the hit Button
*/
void ImageFrameButton::up(eyegui::Layout* pLayout, std::string id) {
}

