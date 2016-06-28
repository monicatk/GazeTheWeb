//============================================================================
// Distributed under the Apache License, Version 2.0.
//============================================================================

#include "ConnectPageArea.h"
#include "src/TwitterApp.h"
#include "src/TwitterClient/imageDownload.h"
#include <string>

/**
* Constructor for the ConnectPageArea Class
* @param[in] pLayout current layout file of GUI
* @param[in] Element a new Element created
*/
ConnectPageArea::ConnectPageArea(eyegui::Layout* pLayout) :Element("content_area", "bricks/ConnectPageArea/connect_Content_Brick.beyegui", pLayout) {
}

/**
* Show function
* lets the instance show up in the interface, also registers the necessary buttons listeners
*/
void ConnectPageArea::show() {
	if (!active)
	{

		Element::show();
		for (int i = 0; i <4; i++)
		{
			textFrames[i] = eyegui::addFloatingFrameWithBrick(pLayout, "bricks/ConnectPageArea/floatingTweetBlock" + std::to_string(i + 1) + ".beyegui", 0.178125f, 0.265f + (0.18f*i), 0.65f, 0.1675f);
			buttonFrames[i] = eyegui::addFloatingFrameWithBrick(pLayout, "bricks/ConnectPageArea/floatingButton" + std::to_string(i + 1) + ".beyegui", 0.178125f, 0.265f + (0.18f*i), 0.65f, 0.1675f);
			eyegui::registerButtonListener(pLayout, std::to_string(i), connectButtonListener);
		}
		buttonFrames[4]=eyegui::addFloatingFrameWithBrick(pLayout, "bricks/ConnectPageArea/receivedButton.beyegui", 0.0f, 0.26f, 0.125f, 0.101f);
		buttonFrames[5]=eyegui::addFloatingFrameWithBrick(pLayout, "bricks/ConnectPageArea/sentButton.beyegui", 0.0f, 0.39f, 0.125f, 0.101f);
		eyegui::registerButtonListener(pLayout, "sentButton", connectButtonListener);
		eyegui::registerButtonListener(pLayout, "receivedButton", connectButtonListener);
	}
	showStatus();
}

/**
* selectContent function
* changes style of Elemten on hand of id
* functions of other classes used
* @param[in] id is ID of selected Contend
*/
void ConnectPageArea::selectContent(std::string id) {
		if (currentlySelected.compare(id))
		{
			if (currentlySelected.compare("none"))
			{
				eyegui::ImageAlignment alignment = eyegui::ImageAlignment::STRETCHED;
				eyegui::replaceElementWithPicture(pLayout, "message" + currentlySelected, "menuebar_elements/tweet_content2.png", alignment);
				eyegui::setStyleOfElement(pLayout, "message" + currentlySelected, "block");
				eyegui::replaceElementWithBlock(pLayout, "rightSide" + currentlySelected, false);
			}
			currentlySelected = id;
			if (id.compare("none"))
			{
				if (index != 0 && stoi(currentlySelected) == 0)
				{
					scrollDown(1);
					selectContent("1");
					return;
				}
				if (stoi(currentlySelected) == 3&& index!=content.Size()-4)
				{
					scrollUp(1);
					selectContent("2");
					return;
				}

				eyegui::ImageAlignment alignment = eyegui::ImageAlignment::STRETCHED;
				eyegui::replaceElementWithPicture(pLayout, "message" + currentlySelected, "Eprojekt_Design/tweetSelected.png", alignment);
				eyegui::setStyleOfElement(pLayout, "message" + currentlySelected, "block");

				eyegui::replaceElementWithPicture(pLayout, "action_button_area", "actionbar_elements/WallActionBar/actionBarWall" +currentlySelected + ".png", alignment);
				eyegui::setStyleOfElement(pLayout, "action_button_area", "block");

				eyegui::replaceElementWithPicture(pLayout, "rightSide" + currentlySelected, "Eprojekt_Design/tweetConnection.png", alignment);

				std::cout << "Result " + id + " has been hit" << std::endl;
				if (stoi(currentlySelected) >= counter)
				{

					return;
				}
				if (receivedMessages)
				{
					TwitterApp::getInstance()->actionButtonArea->changeToMessages(content[index + stoi(currentlySelected)]["sender"]["id_str"].GetString());

				}
				else {
					TwitterApp::getInstance()->actionButtonArea->changeToMessages(content[index + stoi(currentlySelected)]["recipient"]["id_str"].GetString());

				}

				return;
			}
			TwitterApp::getInstance()->actionButtonArea->changeToNone();
		}

}

/**
* switchStatus function
* Changes Status of ConnectPageArea
* on the basis of a bool a different Function will be used
*/
void ConnectPageArea::switchStatus() {
	selectContent("none");
	index = 0;
	receivedMessages = (!receivedMessages); 
	showStatus();
}


/**
* showStatus function
* Changes Status of ConnectPageArea
* on the basis of a bool a different Function will be used
*/
void ConnectPageArea::showStatus() {
	if (receivedMessages)
	{
		content = TwitterApp::getInstance()->getTwitter()->getDirectMessages();
		showMessagesReceived();
	}
	else {
		content = TwitterApp::getInstance()->getTwitter()->getDirectMessagesSent();
		showMessagesSend();
	}
}

/**
* scrollUp function
* Scroll the shown conten up on the basis of an int
* @param[in] i is the int for how much will be scrolled
*/
void ConnectPageArea::scrollUp(int i) {
	selectContent("none");
	if (index + 4 + i <content.Size())
	{
		index += i;
	}
	else {
		index = content.Size() - 4;
	}

	if (receivedMessages)
	{
		showMessagesReceived();
	}
	else {
		showMessagesSend();
	}
}

/**
* scrollDown function
* Scroll the shown conten down on the basis of an int
* @param[in] i is the int for how much will be scrolled
*/
void ConnectPageArea::scrollDown(int i) {
	selectContent("none");
	if (index - i >0)
	{
		index -= i;
	}
	else {
		index = 0;
	}

	if (receivedMessages)
	{
		showMessagesReceived();
	}
	else {
		showMessagesSend();
	}
}

/**
* showMessagesReceived function
* Loads all received messages
* Shows max 4 last Messages
* scrolling in other function possible
*/
void ConnectPageArea::showMessagesReceived() {
	if (TwitterApp::getInstance()->hasConnection()) {
		eyegui::setElementActivity(pLayout, "receivedButton", false);
		eyegui::setElementActivity(pLayout, "sentButton", true);
		counter = 0;
		std::string temp = "\n";
		std::string temp2 = " @";
		for (rapidjson::SizeType i = index; (i < content.Size()) && (i < index + 4); i++) {
			counter++;
			std::string date = content[i]["created_at"].GetString();
			date = date.substr(0, date.size() - 10);
			eyegui::setContentOfTextBlock(pLayout, textboxes[(i - index)], content[i]["sender"]["name"].GetString() + temp2 + content[i]["sender"]["screen_name"].GetString() + temp + date + temp + content[i]["text"].GetString());
		
			
			// Download avatar image:
			std::string str = content[i]["sender"]["profile_image_url"].GetString();

			// Extract file extension
			size_t z = str.rfind('.', str.length());
			std::string ext = str.substr(z + 1, str.length() - z);

			// Set filename using this rule: profile_ + Profile ID + extension
			std::string filename = "img/tmp/profile_" + std::string(content[i]["sender"]["id_str"].GetString()) + "." + ext;

			// Replace substring '_normal.' with '_400x400' to get profile image in the size of 400x400 pixels.
			int indexStr = str.find("_normal.");
			str = str.replace(indexStr, 8, "_400x400.");

			std::string path = CONTENT_PATH + std::string("/") + filename;
			download_image((char*)str.c_str(), (char*)path.c_str());

			eyegui::ImageAlignment alignment = eyegui::ImageAlignment::ORIGINAL;
			eyegui::replaceElementWithPicture(pLayout, "profilePic" + std::to_string(i - index + 1), filename, alignment, false);

		
		}
		std::string fillUp = " ";
		for (int r = counter; r < 4; r++)
		{

			eyegui::setContentOfTextBlock(pLayout, textboxes[(r)], fillUp);
			eyegui::ImageAlignment alignment = eyegui::ImageAlignment::ORIGINAL;
			eyegui::replaceElementWithPicture(pLayout, "profilePic" + std::to_string(r + 1), "img/invisProf.png", alignment, false);
		}
	}
}

/**
* showMessagesSend function
* Loads all send messages
* Shows max 4 last send Messages
* scrolling in other function possible
*/
void ConnectPageArea::showMessagesSend() {
	if (TwitterApp::getInstance()->hasConnection()) {
		eyegui::setElementActivity(pLayout, "receivedButton", true);
		eyegui::setElementActivity(pLayout, "sentButton", false);
		counter = 0;
		std::string temp = "\n";
		std::string temp2 = " @";
		std::string temp1 = "Send to ";
		for (rapidjson::SizeType i = index; (i < content.Size()) && (i < index + 4); i++) {
			counter++;
			std::string date = content[i]["created_at"].GetString();
			date = date.substr(0, date.size() - 10);
			eyegui::setContentOfTextBlock(pLayout, textboxes[(i - index)], temp1 + content[i]["recipient"]["name"].GetString() + temp2 + content[i]["recipient"]["screen_name"].GetString() + temp + date + temp + content[i]["text"].GetString());

			// Download avatar image:
			std::string str = content[i]["recipient"]["profile_image_url"].GetString();

			// Extract file extension
			size_t z = str.rfind('.', str.length());
			std::string ext = str.substr(z + 1, str.length() - z);

			// Set filename using this rule: profile_ + Profile ID + extension
			std::string filename = "img/tmp/profile_" + std::string(content[i]["recipient"]["id_str"].GetString()) + "." + ext;

			// Replace substring '_normal.' with '_400x400' to get profile image in the size of 400x400 pixels.
			int indexStr = str.find("_normal.");
			str = str.replace(indexStr, 8, "_400x400.");

			std::string path = CONTENT_PATH + std::string("/") + filename;
			download_image((char*)str.c_str(), (char*)path.c_str());

			eyegui::ImageAlignment alignment = eyegui::ImageAlignment::ORIGINAL;
			eyegui::replaceElementWithPicture(pLayout, "profilePic" + std::to_string(i - index + 1), filename, alignment, false);
		}
		std::string fillUp = " ";
		for (int r = counter; r < 4; r++)
		{
			eyegui::setContentOfTextBlock(pLayout, textboxes[(r)], fillUp);
			eyegui::ImageAlignment alignment = eyegui::ImageAlignment::ORIGINAL;
			eyegui::replaceElementWithPicture(pLayout, "profilePic" + std::to_string(r + 1), "img/invisProf.png", alignment, false);
		}
	}
}

/**
* hide function
* hides the instances of ConnectPageArea class and delets the floating frames
*/
void ConnectPageArea::hide() {
	if (active)
	{
		currentlySelected = "none";
		for (int i = 0; i < 4; i++)
		{
			eyegui::removeFloatingFrame(pLayout, textFrames[i]);
			eyegui::removeFloatingFrame(pLayout, buttonFrames[i]);
		}
		eyegui::removeFloatingFrame(pLayout, buttonFrames[4]);
		eyegui::removeFloatingFrame(pLayout, buttonFrames[5]);
		index = 0;
	}
	Element::hide();
}

/**
* ~ConnectPageArea function
* no content
*/
ConnectPageArea::~ConnectPageArea() {

}
