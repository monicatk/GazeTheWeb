//============================================================================
// Distributed under the Apache License, Version 2.0.
//============================================================================

#pragma once

#include "src/Interface_Elements/Element.h"
#include "src/Interface_Elements/ContentAreaPages/WallContentArea.h"
#include "src/Buttons/ConnectButton.h"

class ConnectPageArea : public Element {

public:

    ConnectPageArea(eyegui::Layout* pLayout);
    ~ConnectPageArea();
    void virtual show();
    void virtual hide();
    void selectContent(std::string id);
    void switchStatus();
    void showStatus();
    void scrollUp(int i);
    void scrollDown(int i);
    bool receivedMessages = true;
    int index = 0;

private:

    int counter = 0;
    void showMessagesSend();
    void showMessagesReceived();
    rapidjson::Document content;
    std::string currentlySelected = "none";
    std::string textboxes[4] = {
        "textBlock1_1",
        "textBlock2_1",
        "textBlock3_1",
        "textBlock4_1"
    };

    std::shared_ptr<ConnectButton> connectButtonListener = std::shared_ptr<ConnectButton>(new ConnectButton);
    int buttonFrames[6];
    int textFrames[4];
};
