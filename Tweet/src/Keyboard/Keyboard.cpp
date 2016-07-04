//============================================================================
// Distributed under the Apache License, Version 2.0.
//============================================================================

#include "Keyboard.h"
#include "src/TwitterApp.h"
#include <string>

/**
* Constructor for the Keyboard
* @param[in] pLayout current layout file of GUI
* @param[in] pLayout2 current layout of the Keyboard
*/
Keyboard::Keyboard(eyegui::Layout* pLayout, eyegui::Layout* pLayout2) {
    this->pLayout = pLayout;
    this->pLayout2 = pLayout2;
    init();
}

//Singletons have to be set to 0, when they are not instanciated yet
Keyboard* Keyboard::instance = 0;

/**
* Creates an instance of the Keyboard
* only one instance can exist
* @param[in] pLayout current layout file of GUI
* @param[in] pLayout2 current layout of the Keyboard
*/
//-------------------------------------------------------------------------------------------------------------
Keyboard* Keyboard::createInstance(eyegui::Layout* pLayout, eyegui::Layout* pLayout2) {
    //check if an instance already exists
    if (!instance)
    {
        instance = new Keyboard(pLayout, pLayout2);
        return instance;
    }
    //if there is already one give back this warning
    std::cout << "There cant be two instances of Keyboard" << std::endl;
    return instance;
}

/**
* returns the current  instance of the Keyboard, if it exists already
*/
Keyboard* Keyboard::getInstance() {
    if (!instance)
    {
        std::cout << "A instance doesnt exist yet, pls use createInstance(int, int)" << std::endl;
    }
    return instance;
}

//-------------------------------------------------------------------------------------------------------------

/**
* Getter function for Cas Variable
* Cas Variable for determining which function should be used = tweet or respond or for pointer
*/
int Keyboard::getCas() {
    return cas;
}

/**
* Setter function for Cas Variable
* Cas Variable for determining which function should be used = tweet or respond or for pointer
*/
void Keyboard::setCas(int cas) {
    this->cas = cas;
}

/**
* Getter function for tweetid
* tweetid is used by respond function in twitter app
* TweetId for which the respond function answers
*/
std::string Keyboard::getId() {
    return tweetid;
}

/**
* Setter function for tweetid
* tweetid is used by respond function in twitter app
* TweetId for which the respond function answers
*/
void Keyboard::setId(std::string tweetid) {
    this->tweetid = tweetid;
}

/**
* Innitialising of the variables
*/
void Keyboard::init() {

    // UpperCase Keys first shown
    keys = eyegui::addFloatingFrameWithBrick(pLayout2, "bricks/Keyboard/keyboardUpper2.beyegui", PosX, PosY, WIDTH, HEIGHT, true);

    // Lowercase Keys created und made invisible, visibility can be chaned through buttons
    keys2 = eyegui::addFloatingFrameWithBrick(pLayout2, "bricks/Keyboard/keyboardLower2.beyegui", PosX, PosY, WIDTH, HEIGHT, true);
    keys3 = eyegui::addFloatingFrameWithBrick(pLayout2, "bricks/Keyboard/keyboardYesNo2.beyegui", PosX, PosY, WIDTH, HEIGHT, false);
    numbers = eyegui::addFloatingFrameWithBrick(pLayout2, "bricks/Keyboard/keyboardNumbers.beyegui", PosX, PosY, WIDTH, HEIGHT, false);
    specialKeys = eyegui::addFloatingFrameWithBrick(pLayout2, "bricks/Keyboard/keyboardSpecialCharacters.beyegui", PosX, PosY, WIDTH, HEIGHT, false);
    keysSymbol = eyegui::addFloatingFrameWithBrick(pLayout2, "bricks/Keyboard/keyboardSymbols.beyegui", PosX, PosY, WIDTH, HEIGHT, false);
    functionSymbol = eyegui::addFloatingFrameWithBrick(pLayout2, "bricks/Keyboard/keyboardFunctions.beyegui", PosX, PosY, WIDTH, HEIGHT, false);

    // TextBlock must be created last or there will be visibility errors
    // WordCompletion
    Word1 = eyegui::addFloatingFrameWithBrick(pLayout2, "bricks/Keyboard/keyboardText.beyegui", 0.1f, 0.235f, 0.7f, HEIGHT, true);
    Word2 = eyegui::addFloatingFrameWithBrick(pLayout2, "bricks/Keyboard/keyboardText2.beyegui", 0.4f, 0.235f, 0.7f, HEIGHT, true);
    Word3 = eyegui::addFloatingFrameWithBrick(pLayout2, "bricks/Keyboard/keyboardText3.beyegui", 0.7f, 0.235f, 0.7f, HEIGHT, true);

    // trie.loadDict("../src/Keyboard/dict.txt");
    dict = CONTENT_PATH + std::string("/dict/") + "eng.txt"; // "../src/Keyboard/eng.txt";
    trie.loadDict(dict);

    Listener(); // initiating KeyListener for the Keyboard class

    currentCursorPos = 0; // since there is no text yet, set cursor position to 0
    isUpper= true;
}

/**
* tweet function of Twitterap
* Tweeting string variable "ausgabe" through TwitterApp
*/
void Keyboard::tweet() {
    TwitterApp::getInstance()->getTwitter()->statusUpdate(ausgabe);
    TwitterApp::getInstance()->wallContentArea->updateNewsFeed(true);
    abort();
}

/**
* respond function of Twitterapp
* Responding to a Tweet with sring "ausgabe" and "tweetid"
*/
void Keyboard::respond() {
    //std::cout << tweetid << std::endl;
    TwitterApp::getInstance()->getTwitter()->reply(ausgabe, tweetid);
    TwitterApp::getInstance()->wallContentArea->updateNewsFeed(false);
    abort();
}

//------------------------------------------------------------------

/**
* Setting string to Pointer, so other classes can use keyboard results
* @param[in] String wich goes to pointer
*/
void Keyboard::setPointer(std::string& ausgabeText) {
    pointer = &ausgabeText;
}

/**
* Sets the valou of the pointer to the ausgabe
* used in the case of Cas == 3
*/
void Keyboard::setPointerValue() {
    pointer->assign(ausgabe);
    abort();
}
//------------------------------------------------------------------

/**
* Write one char to ausgabe-string
* Displaying of string will take place in 'insertCursorAndDisplayText'
* @param[in] pLayout current layout file
* @param[in] key char which has been pressed on the keyboard
* @todo find better layout for keyboard!
*/
void Keyboard::write(const char key) {
    if (ausgabe.length()<140) {

        //-------------------------------------
        //WordCompletion
        if (key == ' ') {
            tempWord = "";
            eyegui::setContentOfTextBlock(pLayout2, "Word1", "");
            eyegui::setContentOfTextBlock(pLayout2, "Word2", "");
            eyegui::setContentOfTextBlock(pLayout2, "Word3", "");
        }
        else {
            tempWord += key;
            showWordComp();
        }


        //-------------------------------------

        ausgabe.insert(currentCursorPos, 1, key);
        eyegui::setStyleOfElement(pLayout2, "keyboardtext", "keyboard3");
        if (ausgabe.size() > 1 && (((ausgabe.size() - 1) % 70) == 0)) {
            std::cout << "There will be a new line" << std::endl;
            ausgabe.insert(currentCursorPos + 1, 1, '\n');
            insertCursorAndDisplayText(2);
        }
        else {
            insertCursorAndDisplayText(1);
        }

        //if Upper Case Letter then show LowerCase laout after
        if (isupper(key)) {
            LowerKeys();
        }
    }
    else {
        eyegui::setStyleOfElement(pLayout2, "keyboardtext", "keyboard2");
    }
}



//---------------------------------------------------------------------------------------

/**
* Copy of Write, without the wordcompletion feature
* used by the Wordcompletion button writing
* Write one char to ausgabe-string
* Displaying of string will take place in 'insertCursorAndDisplayText'
* @param[in] key char which has been pressed on the keyboard
* @todo find better layout for keyboard!
*/
void Keyboard::write2(char key) {
    if (ausgabe.length()<140) {


        //Chars not included in Eye-Gui, result in error. if clausel should chatch execptions
        if (key == 'ö' || key == 'ä' || key == 'ü' || key == 'Ö' || key == 'Ä' || key == 'Ü' || key == '§' || key == 'ß' || key == '?' || key == '`'
            || key == '°' || key == '¤' || key == 'â' || key == 'ê' || key == 'û' || key == 'ô' || key == 'Â' || key == 'Ê' || key == 'Û' || key == 'Ô'
            || key == 'î' || key == 'Î' || key == 'ú' || key == 'Ú' || key == 'é' || key == 'É' || key == 'í' || key == 'Í' || key == 'ó' || key == 'Ó'
            || key == 'á' || key == 'Á' || key == 'ý' || key == 'Ý') {
            key = '*';
        }

        //-------------------------------------
        //WordCompletion
        if (key == ' ') {
            tempWord = "";
            eyegui::setContentOfTextBlock(pLayout2, "Word1", "");
            eyegui::setContentOfTextBlock(pLayout2, "Word2", "");
            eyegui::setContentOfTextBlock(pLayout2, "Word3", "");
        }
        else {
            tempWord += key;
        }

        //-------------------------------------

        ausgabe.insert(currentCursorPos, 1, key);
        eyegui::setStyleOfElement(pLayout2, "keyboardtext", "keyboard3");
        if (ausgabe.size() > 1 && (((ausgabe.size() - 1) % 70) == 0)) {
            std::cout << "There will be a new line" << std::endl;
            ausgabe.insert(currentCursorPos + 1, 1, '\n');
            insertCursorAndDisplayText(2);
        }
        else {
            insertCursorAndDisplayText(1);
        }

        //if Upper Case Letter then show LowerCase laout after
        if (isupper(key)) {
            LowerKeys();
        }
    }
    else {
        eyegui::setStyleOfElement(pLayout2, "keyboardtext", "keyboard2");
    }
}

//---------------------------------------------------------------------------------------

/**
 * Write method using strings to handle multibyte characters.
 * @param[in] key a string containing the character that should be added to the "ausgabe" string.
 * @todo Reactivate automatic case switching.!
 *
 */
void Keyboard::write3(std::string key){
    if (ausgabe.length()<140) {
        if (key == " ") {
            tempWord = "";
            eyegui::setContentOfTextBlock(pLayout2, "Word1", "");
            eyegui::setContentOfTextBlock(pLayout2, "Word2", "");
            eyegui::setContentOfTextBlock(pLayout2, "Word3", "");
        }
        else {
            tempWord += key;
            if (useWordComp) {
                showWordComp();
            }
        }
        ausgabe.insert(currentCursorPos, key);
        eyegui::setStyleOfElement(pLayout2, "keyboardtext", "keyboard3");
        if (ausgabe.size() > 1 && (((ausgabe.size() - 1) % 70) == 0)) {
            std::cout << "There will be a new line" << std::endl;
            ausgabe.insert(currentCursorPos + 1, 1, '\n');
        }
        else {
          if(key.size() ==2 && (key.at(0) &  0b11100000) ==  0b11000000 	// Next byte will be multibyte char; move two spaces
             && (key.at(1) & 0b11000000) == 0b10000000){
            cout << "multibyte with size" << ausgabe.size()<< endl;
            cout << "currentCursorPos" << currentCursorPos << endl;
            insertCursorAndDisplayText(2);

          }
          else{
            insertCursorAndDisplayText(1);}
        }

        if (isUpper) {
            LowerKeys();
        }
    }
}

/**
* Move current cursor position to position + offset, then display text
* Normal text will not be overwritten by cursor, tempAusgabe will be used.
* @param[in] offset offset to current cursor position (negative for moving backwards, positive for writing or moving forward)
*
*/
void Keyboard::insertCursorAndDisplayText(int offset) {
    std::string tempAusgabe = ausgabe;
    tempAusgabe = ausgabe;
    currentCursorPos += offset;
    tempAusgabe.insert(currentCursorPos, "|");
    eyegui::setContentOfTextBlock(pLayout2, "keyboardtext", tempAusgabe);
}

/**
* Move cursor in one direction by using the arrow keys. This method distinguishes between one byte characters and multibyte characters.
* Actual displaying of the text will take place in 'insertCursorAndDisplayText'-Method

* @param[in] direction if 'true' move cursor to right, if 'false' move cursor to left.
*
* @see Keyboard::insertCursorAndDisplayText
* @todo Disable buttons if cursor reached end of the text (front or back end)
* @todo Catch multibyte characters with more than 2 bytes.
*/

void Keyboard::moveCursor(bool direction) {
    if (direction && (size_t) currentCursorPos<ausgabe.size()) {						//Move cursor to the right

    if( (ausgabe.at(currentCursorPos) & 0b10000000) == 0) {						// Next byte will be single byte char; move only one space
          insertCursorAndDisplayText(1);
      }
       if( currentCursorPos<ausgabe.size()-1 && (ausgabe.at(currentCursorPos) & 0b11100000)== 0b11000000 	// Next byte will be multibyte char; move two spaces
        && (ausgabe.at(currentCursorPos +1) & 0b11000000) == 0b10000000){
        insertCursorAndDisplayText(2);
      }
    }
    else if (direction == false && currentCursorPos>0) {							//Move cursor to the left

        if ( (ausgabe.at(currentCursorPos -1) & 0b10000000) == 0)						//Next char will be single-byte
        {
        insertCursorAndDisplayText(-1);
        }
        else if (
          currentCursorPos > 1 &&										//Next char will be multi byte
          (ausgabe.at(currentCursorPos -2) & 0b11100000) == 0b11000000
          && (ausgabe.at(currentCursorPos -1) & 0b11000000) == 0b10000000)
        {
          insertCursorAndDisplayText(-2);
        }
        else
        {
          // More than two bytes characters see @todo
        }
    }
}

/**
* Delete char last written at currentCursorPosition
* If char in front of cursor is multibyte char, two bytes must be deleted.

*/
void Keyboard::deleteKey() {
    //-------------------------------
    //Wordcomp
    if (tempWord.length() > 0) {
        tempWord.erase(tempWord.end() - 1);
        if (tempWord.length() == 0) {
            eyegui::setContentOfTextBlock(pLayout2, "Word1", "");
            eyegui::setContentOfTextBlock(pLayout2, "Word2", "");
            eyegui::setContentOfTextBlock(pLayout2, "Word3", "");
        }
        else {
            showWordComp();
        }
    }
    //-------------------------------

    if (currentCursorPos>0) {
          if((ausgabe.at(currentCursorPos-1) & 0b10000000) == 0 ){						//if char to be deleted is single byte
        ausgabe.erase(currentCursorPos - 1, 1);
        insertCursorAndDisplayText(-1);
          }
          else if((currentCursorPos>1) && (ausgabe.at(currentCursorPos -2) & 0b11100000) == 0b11000000	//if char to be deleted is multibyte delete two bytes instead of one
          && (ausgabe.at(currentCursorPos -1) & 0b11000000) == 0b10000000){
        ausgabe.erase(currentCursorPos-2,2);
        insertCursorAndDisplayText(-2);
          }

        if (ausgabe.length() < 140) {
            eyegui::setStyleOfElement(pLayout2, "keyboardtext", "keyboard3");
        }
    }
}

/**

 * This method changes the button press speed of the keyboard.
 * Changes will be reset on closing the keyboard.
 * @param[in] accelerate Boolean wether button press speed should be accelerated or not.
 *
 *
 */

void Keyboard::changeSpeed(bool accelerate) {

    if (accelerate && speed>0.2f) {
        speed -= 0.2f;
    }
    else if (!accelerate && speed<2.4f) {
        speed += 0.2f;
    }
    std::ostringstream convertFloat;
    convertFloat << speed;
    std::string speedstring = convertFloat.str();
    speedstring.append("f");
    eyegui::setValueOfConfigAttribute(TwitterApp::getInstance() -> getGUI(), "button-threshold-increase-duration", speedstring);
}

/**
 * Resets button-press speed to default value (1.0).
 */
void Keyboard::resetSpeed() {
    eyegui::setValueOfConfigAttribute(TwitterApp::getInstance()->getInstance() -> getGUI(), "button-threshold-increase-duration", "1.0");
}

/**
* Abort functions resets all variables for next use of Keyboard
* Keyboard Layout deactivated
* Visibitlity of Layout2 = false
*/
void Keyboard::abort() {
    //Delete Block of Class
    cas = 0;
    ausgabe = "";
    currentCursorPos = 0;
    tweetid = "";
    tempWord = "";
    word1 = "";
    word2 = "";
    word3 = "";
    eyegui::setContentOfTextBlock(pLayout2, "Word1", "");
    eyegui::setContentOfTextBlock(pLayout2, "Word2", "");
    eyegui::setContentOfTextBlock(pLayout2, "Word3", "");

    clipboard = "";
    pointer = NULL;
    resetSpeed();
    eyegui::setContentOfTextBlock(pLayout2, "keyboardtext", "Ausgabetext");
    eyegui::setVisibilityOfLayout(pLayout, true);
    eyegui::setVisibilityOfLayout(pLayout2, false);
}

/**
* Functions activates Keyboard
* Keyboard Layout activated
* Visibitlity of Layout2 = true
*/
void Keyboard::activate() {
    UpperKeys();
    eyegui::setVisibilityOfLayout(pLayout, false);
    eyegui::setVisibilityOfLayout(pLayout2, true);
}

/**
* Functions activates UpperKeys Frames of the Keyboard
* Setting Visibility of Uppercase Keys to true
* UpperCase = visible
*/
void Keyboard::UpperKeys() {
    isUpper=true;
    eyegui::setVisibilityOFloatingFrame(pLayout2, keys, true, false, false);
    eyegui::setVisibilityOFloatingFrame(pLayout2, keys2, false, false, false);
    eyegui::setVisibilityOFloatingFrame(pLayout2, keys3, false, false, false);
    eyegui::setVisibilityOFloatingFrame(pLayout2, numbers,false, false, false);
    eyegui::setVisibilityOFloatingFrame(pLayout2, specialKeys, false, false, false);
    eyegui::setVisibilityOFloatingFrame(pLayout2, keysSymbol, false, false, false);
    eyegui::setVisibilityOFloatingFrame(pLayout2, functionSymbol, false, false, false);
}

/**
* Functions activates LowerKeys Frames of the Keyboard
* Setting Visibility of Lowercase Keys to truee
* LowerCase = visible
*/
void Keyboard::LowerKeys() {
    isUpper=false;
    eyegui::setVisibilityOFloatingFrame(pLayout2, keys, false, false, false);
    eyegui::setVisibilityOFloatingFrame(pLayout2, keys2, true, false, false);
    eyegui::setVisibilityOFloatingFrame(pLayout2, keys3, false, false, false);
    eyegui::setVisibilityOFloatingFrame(pLayout2, numbers,false, false, false);
    eyegui::setVisibilityOFloatingFrame(pLayout2, specialKeys, false, false, false);
    eyegui::setVisibilityOFloatingFrame(pLayout2, keysSymbol, false, false, false);
    eyegui::setVisibilityOFloatingFrame(pLayout2, functionSymbol, false, false, false);
}

/**
* Functions activates YesNoKeys Frames of the Keyboard
* Setting Visibility of YesNoKeys Keys to true
* YesNo = visible
*/
void Keyboard::YesNoKeys() {
    isUpper=false;
    eyegui::setVisibilityOFloatingFrame(pLayout2, keys, false, false, false);
    eyegui::setVisibilityOFloatingFrame(pLayout2, keys2, false, false, false);
    eyegui::setVisibilityOFloatingFrame(pLayout2, keys3, true, false, false);
    eyegui::setVisibilityOFloatingFrame(pLayout2, numbers,false, false, false);
    eyegui::setVisibilityOFloatingFrame(pLayout2, specialKeys, false, false, false);
    eyegui::setVisibilityOFloatingFrame(pLayout2, keysSymbol, false, false, false);
    eyegui::setVisibilityOFloatingFrame(pLayout2, functionSymbol, false, false, false);
}

void Keyboard::numberKeys(){
    isUpper=false;
    eyegui::setVisibilityOFloatingFrame(pLayout2, keys, false, false, false);
    eyegui::setVisibilityOFloatingFrame(pLayout2, keys2, false, false, false);
    eyegui::setVisibilityOFloatingFrame(pLayout2, keys3, false, false, false);
    eyegui::setVisibilityOFloatingFrame(pLayout2, numbers,true, false, false);
    eyegui::setVisibilityOFloatingFrame(pLayout2, specialKeys, false, false, false);
    eyegui::setVisibilityOFloatingFrame(pLayout2, keysSymbol, false, false, false);
    eyegui::setVisibilityOFloatingFrame(pLayout2, functionSymbol, false, false, false);
}

void Keyboard::specialKeysFunc(){
    isUpper=false;
    eyegui::setVisibilityOFloatingFrame(pLayout2, keys, false, false, false);
    eyegui::setVisibilityOFloatingFrame(pLayout2, keys2, false, false, false);
    eyegui::setVisibilityOFloatingFrame(pLayout2, keys3, false, false, false);
    eyegui::setVisibilityOFloatingFrame(pLayout2, numbers,false, false, false);
    eyegui::setVisibilityOFloatingFrame(pLayout2, specialKeys, true, false, false);
    eyegui::setVisibilityOFloatingFrame(pLayout2, keysSymbol, false, false, false);
    eyegui::setVisibilityOFloatingFrame(pLayout2, functionSymbol, false, false, false);
}

void Keyboard::symbolKeys(){
    isUpper=false;
    eyegui::setVisibilityOFloatingFrame(pLayout2, keys, false, false, false);
    eyegui::setVisibilityOFloatingFrame(pLayout2, keys2, false, false, false);
    eyegui::setVisibilityOFloatingFrame(pLayout2, keys3, false, false, false);
    eyegui::setVisibilityOFloatingFrame(pLayout2, numbers,false, false, false);
    eyegui::setVisibilityOFloatingFrame(pLayout2, specialKeys, false, false, false);
    eyegui::setVisibilityOFloatingFrame(pLayout2, keysSymbol, true, false, false);
    eyegui::setVisibilityOFloatingFrame(pLayout2, functionSymbol, false, false, false);
}

void Keyboard::functionKeys(){
    isUpper=false;
    eyegui::setVisibilityOFloatingFrame(pLayout2, keys, false, false, false);
    eyegui::setVisibilityOFloatingFrame(pLayout2, keys2, false, false, false);
    eyegui::setVisibilityOFloatingFrame(pLayout2, keys3, false, false, false);
    eyegui::setVisibilityOFloatingFrame(pLayout2, numbers,false, false, false);
    eyegui::setVisibilityOFloatingFrame(pLayout2, specialKeys, false, false, false);
    eyegui::setVisibilityOFloatingFrame(pLayout2, keysSymbol, false, false, false);
    eyegui::setVisibilityOFloatingFrame(pLayout2, functionSymbol, true, false, false);
}

//AutoComplete Section---------------------------------------------------------------------
/*
Todo
--Find and remove Bugs
--in the End Code cleaning
*/
//Autocomplete of tempWord and showing in Textblocks

/**
* Functions sets 3 Words of the Trie wordcomplition
* to the Word Bricks
*/
void Keyboard::showWordComp() {

    v = trie.autocomplete(tempWord);

    if (v.size() >= 3) {
        Wcount = 3;
    }

    if (v.size() == 1) {

        v[0] = replaceChar2(v[0]);
        eyegui::setContentOfTextBlock(pLayout2, "Word1", v[0]);
        eyegui::setContentOfTextBlock(pLayout2, "Word2", "");
        eyegui::setContentOfTextBlock(pLayout2, "Word3", "");

        word1 = v[0];
        word2 = "";
        word3 = "";
    }
    else if (v.size() == 2) {

        v[0] = replaceChar2(v[0]);
        v[1] = replaceChar2(v[1]);

        eyegui::setContentOfTextBlock(pLayout2, "Word1", v[0]);
        eyegui::setContentOfTextBlock(pLayout2, "Word2", v[1]);
        eyegui::setContentOfTextBlock(pLayout2, "Word3", "");

        word1 = v[0];
        word2 = v[1];
        word3 = "";
    }
    else if (v.size() >= 3) {

        v[0] = replaceChar2(v[0]);
        v[1] = replaceChar2(v[1]);
        v[2] = replaceChar2(v[2]);

        eyegui::setContentOfTextBlock(pLayout2, "Word1", v[0]);
        eyegui::setContentOfTextBlock(pLayout2, "Word2", v[1]);
        eyegui::setContentOfTextBlock(pLayout2, "Word3", v[2]);
        word1 = v[0];
        word2 = v[1];
        word3 = v[2];
    }
}

/**
* Function which replaces chars in a string
* To be replaced Chars are all chars not incuded in the EyeGui
* @param[in] String wich will be checked for chars not included in eyegui
*/
std::string Keyboard::replaceChar(std::string Word) {
    std::string tempWord2;

    // Loop through all the characters of string
    for (int i = 0; i < Word.length(); ++i)
    {
        //current char
        char key2 = Word[i];

        //Chars not included in Eye-Gui, result in error. if clausel should chatch execptions
        if (key2 == 'ö' || key2 == 'ä' || key2 == 'ü' || key2 == 'Ö' || key2 == 'Ä' || key2 == 'Ü' || key2 == '§' || key2 == 'ß' || key2 == '´' || key2 == '`'
            || key2 == '°' || key2 == '€' || key2 == 'â' || key2 == 'ê' || key2 == 'û' || key2 == 'ô' || key2 == 'Â' || key2 == 'Ê' || key2 == 'Û' || key2 == 'Ô'
            || key2 == 'î' || key2 == 'Î' || key2 == 'ú' || key2 == 'Ú' || key2 == 'é' || key2 == 'É' || key2 == 'í' || key2 == 'Í' || key2 == 'ó' || key2 == 'Ó'
            || key2 == 'á' || key2 == 'Á' || key2 == 'ý' || key2 == 'Ý') {
            key2 = '*';
        }

        // Add the current character to temp
        tempWord2 += key2;
    }
    return tempWord2;
}

std::string Keyboard::replaceChar2(std::string Word) {
    std::string tempWord2;

    // Loop through all the characters of string
    for (int i = 0; i < Word.length(); ++i)
    {
        // Current char
        string key2(1, Word[i]);

        // Chars not included in Eye-Gui, result in error. if clausel should chatch execptions
        if (key2 == "ö") {
            key2 = u8"ö";
        }
        if (key2 == "Ö") {
            key2 = u8"Ö";
        }
        if (key2 == "ä") {
            key2 = u8"ä";
        }
        if (key2 == "Ä") {
            key2 = u8"Ä";
        }
        if (key2 == "ü") {
            key2 = u8"ü";
        }
        if (key2 == "Ü") {
            key2 = u8"Ü";
        }
        if (key2 == "§") {
            key2 = u8"§";
        }
        if (key2 == "ß") {
            key2 = u8"ß";
        }
        if (key2 == "´") {
            key2 = u8"´";
        }
        if (key2 == "`") {
            key2 = u8"`";
        }
        if (key2 == "°") {
            key2 = u8"°";
        }
        if (key2 == "€") {
            key2 = u8"€";
        }
        if (key2 == "â") {
            key2 = u8"â";
        }
        if (key2 == "ê") {
            key2 = u8"ê";
        }
        if (key2 == "û") {
            key2 = u8"û";
        }
        if (key2 == "ô") {
            key2 = u8"ô";
        }
        if (key2 == "Â") {
            key2 = u8"Â";
        }
        if (key2 == "Ê") {
            key2 = u8"Ê";
        }
        if (key2 == "Û") {
            key2 = u8"Û";
        }
        if (key2 == "Ô") {
            key2 = u8"Ô";
        }
        if (key2 == "î") {
            key2 = u8"î";
        }
        if (key2 == "Î") {
            key2 = u8"Î";
        }
        if (key2 == "ú") {
            key2 = u8"ú";
        }
        if (key2 == "Ú") {
            key2 = u8"Ú";
        }
        if (key2 == "é") {
            key2 = u8"é";
        }
        if (key2 == "É") {
            key2 = u8"É";
        }
        if (key2 == "í") {
            key2 = u8"í";
        }
        if (key2 == "Í") {
            key2 = u8"Í";
        }
        if (key2 == "ó") {
            key2 = u8"ó";
        }
        if (key2 == "Ó") {
            key2 = u8"Ó";
        }
        if (key2 == "á") {
            key2 = u8"á";
        }
        if (key2 == "Á") {
            key2 = u8"Á";
        }
        if (key2 == "ý") {
            key2 = u8"ý";
        }
        if (key2 == "Ý") {
            key2 = u8"Ý";
        }
        if (key2 == "*") {
            key2 = u8"*";
        }
        if (key2 == "'") {
            key2 = u8"'";
        }

        // Add the current character to temp
        tempWord2 += key2;
    }
    return tempWord2;
}

std::string Keyboard::replaceChar3(std::string Word) {
    std::string tempWord2;
    // Loop through all the characters of string
    for (int i = 0; i < Word.length(); ++i)
    {
        // Current char
        string key2(1, Word[i]);
        std::cout << "'Schleife der ReplaceChar3  " << tempWord2 << std::endl;
        // Chars not included in Eye-Gui, result in error. if clausel should chatch execptions
        if (key2 == u8"ä") {
            std::cout << "Buchstabe wird ersetzt " << tempWord2 << std::endl;
            key2 = "ä";
        }
        // Add the current character to temp
        tempWord2 += key2;
    }
    return tempWord2;
}

/**
* delete Key without  Wordcompletion function
*/
void Keyboard::deleteKey2() {
    if (currentCursorPos>0) {
        ausgabe.erase(currentCursorPos - 1, 1);
        insertCursorAndDisplayText(-1);

        if (ausgabe.length() < 140) {
            eyegui::setStyleOfElement(pLayout2, "keyboardtext", "keyboard3");
        }
    }
}

/**
* Button function to WordmoveRight
* Moves one word further of the Trie Wordcompletion
*/
void Keyboard::WordmoveRight() {
    if (Wcount >= 3 && (size_t) Wcount<v.size()) {
        Wcount = Wcount + 1;

        v[Wcount - 3]= replaceChar2(v[Wcount - 3]);
        v[Wcount - 2]= replaceChar2(v[Wcount - 2]);
        v[Wcount - 1]= replaceChar2(v[Wcount - 1]);

        eyegui::setContentOfTextBlock(pLayout2, "Word1", v[Wcount - 3]);
        eyegui::setContentOfTextBlock(pLayout2, "Word2", v[Wcount - 2]);
        eyegui::setContentOfTextBlock(pLayout2, "Word3", v[Wcount - 1]);
        word1 = v[Wcount - 3];
        word2 = v[Wcount - 2];
        word3 = v[Wcount - 1];
    }
}

/**
* Button function to WordmoveLeft
* Moves one word back of the Trie Wordcompletion
*/
void Keyboard::WordmoveLeft() {
    if (Wcount > 3) {
        Wcount = Wcount - 1;

        v[Wcount - 3] = replaceChar2(v[Wcount - 3]);
        v[Wcount - 2] = replaceChar2(v[Wcount - 2]);
        v[Wcount - 1] = replaceChar2(v[Wcount - 1]);
        eyegui::setContentOfTextBlock(pLayout2, "Word1", v[Wcount - 3]);
        eyegui::setContentOfTextBlock(pLayout2, "Word2", v[Wcount - 2]);
        eyegui::setContentOfTextBlock(pLayout2, "Word3", v[Wcount - 1]);
        word1 = v[Wcount - 3];
        word2 = v[Wcount - 2];
        word3 = v[Wcount - 1];
    }
}

/**
* Button function writes Word from Wordcompletion-buttons to "ausgabe"
* 3 Wordcompletion-Buttons use this function
* @param[in] Wordcase variable decides wich word will be writen to ausgabe
*/
void Keyboard::writeWordComp(int Wordcase){
    useWordComp = false;
    if (Wordcase == 1 && word1.length() > 0) {
        for (size_t i = 0; i < tempWord.length(); i++) {
            deleteKey2();
        }

        for (char& c : word1) {
            //write2(c);
            string s(1, c);
            write3(s);
        }
        write3(" ");
        tempWord = "";
        word1 = "";
        word2 = "";
        word3 = "";
    }

    if (Wordcase == 2 && word2.length() > 0) {
        for (size_t i = 0; i < tempWord.length(); i++) {
            deleteKey2();
        }
        for (char& c : word2) {

            string s(1, c);
            write3(s);
        }
        write3(" ");
        tempWord = "";
        word1 = "";
        word2 = "";
        word3 = "";
    }

    if (Wordcase ==3 && word3.length() > 0) {
        for (size_t i = 0; i < tempWord.length(); i++) {
            deleteKey2();
        }
        for (char& c : word3) {

            string s(1, c);
            write3(s);
        }
        write3(" ");
        tempWord = "";
        word1 = "";
        word2 = "";
        word3 = "";
    }
    useWordComp = true;
}

//----------------------------------------------------------------------------------

// More Keyboard function for User
/**
* Setting function of the clipboard
*/
void Keyboard::addTexttoClipboard() {
    clipboard = ausgabe;
}

/**
* Function writes clipboard to ausgabe
*/
void Keyboard::addClipboardtoOut() {
    if (clipboard.length() > 0) {
        string s{};
        for (char& c : clipboard) {
            s.push_back(c);
            write3(s);
            s.pop_back();
        }
    }
}

/**
* Function adds ausgabe as a new line to the Dictionary
*/
void Keyboard::addLinetoDict() {
    trie.addLinetoDict(dict,ausgabe);
}

/**
* Function deletes ausgabe line from the Dictionary, if posssible
*/
void Keyboard::deleteLineinDict() {
    trie.deleteLineinDict(dict, ausgabe);
    trie = Trie("");
    trie.loadDict(dict);

    ausgabe = "";
    currentCursorPos = 0;
    tempWord = " ";
    word1 = "";
    word2 = "";
    word3 = "";
    eyegui::setContentOfTextBlock(pLayout2, "keyboardtext", "");

    eyegui::setContentOfTextBlock(pLayout2, "Word1", "");
    eyegui::setContentOfTextBlock(pLayout2, "Word2", "");
    eyegui::setContentOfTextBlock(pLayout2, "Word3", "");

    showWordComp();
}

/**
* Function which clears all Input in Keyboard
*/
void Keyboard::clearInput() {
    ausgabe = "";
    currentCursorPos = 0;
    tempWord = " ";
    word1 = "";
    word2 = "";
    word3 = "";
    eyegui::setContentOfTextBlock(pLayout2, "keyboardtext", "");

    eyegui::setContentOfTextBlock(pLayout2, "Word1", "");
    eyegui::setContentOfTextBlock(pLayout2, "Word2", "");
    eyegui::setContentOfTextBlock(pLayout2, "Word3", "");
}

/**
* Function which loads a different Dictionary in the programm
* 4 Languages: English, German, French, Dutch, all with the most coomen 10.000 words used
* @param[in] Language integer for the different cases
*/
void Keyboard::changeDict(int lang) {
    if (lang == 0) {
        dict = "../src/Keyboard/eng.txt";
    }
    if (lang == 1) {
        dict = "../src/Keyboard/ger.txt";
    }
    if (lang == 2) {
        dict = "../src/Keyboard/french.txt";
    }
    if (lang == 3) {
        dict = "../src/Keyboard/dutch.txt";
    }

    trie = Trie("");
    trie.loadDict(dict);

    tempWord = " ";
    word1 = "";
    word2 = "";
    word3 = "";

    eyegui::setContentOfTextBlock(pLayout2, "Word1", "");
    eyegui::setContentOfTextBlock(pLayout2, "Word2", "");
    eyegui::setContentOfTextBlock(pLayout2, "Word3", "");

    v = trie.autocomplete(tempWord);

    // Bug: has some problems with the first char after loading new Dictionary
    // so we write one char and delete it
    write3(" ");
    deleteKey();
}

//----------------------------------------------------------------------------------

/**
* Listener for all Buttons in the KeyboarClass
*/
void Keyboard::Listener() {

    // UpperKeys
    eyegui::registerButtonListener(pLayout2, "keyA", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyB", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyC", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyD", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyE", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyF", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyG", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyH", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyI", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyJ", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyK", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyL", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyM", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyN", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyO", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyP", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyQ", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyR", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyS", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyT", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyU", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyV", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyW", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyX", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyY", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyZ", KeyboardButtonListener);

    eyegui::registerButtonListener(pLayout2, "keyDelete", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyLeer", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyLeer2", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyLeer3", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyLeer4", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyLeer5", KeyboardButtonListener);
    //eyegui::registerButtonListener(pLayout2, "keyLeer6", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyEnter", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyEnter2", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyEnter3", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyEnter4", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyEnter5", KeyboardButtonListener);
    //eyegui::registerButtonListener(pLayout2, "keyEnter6", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyAbort", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyLower", KeyboardButtonListener);

    //----------------------------------------------------------------------------------
    // LowerKeys
    eyegui::registerButtonListener(pLayout2, "keya", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyb", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyc", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyd", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keye", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyf", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyg", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyh", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyi", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyj", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyk", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyl", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keym", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyn", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyo", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyp", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyq", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyr", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keys", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyt", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyu", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyv", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyw", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyx", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyy", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyz", KeyboardButtonListener);

    eyegui::registerButtonListener(pLayout2, "keyDelete2", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyDelete3", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyDelete4", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyDelete5", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyDelete6", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyLeer2", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyEnter2", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyUpper", KeyboardButtonListener);

    // Special Characters
    eyegui::registerButtonListener(pLayout2, "keyHashtag", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2,"keyHashtag2", KeyboardButtonListener);

    // Punctuation
    eyegui::registerButtonListener(pLayout2,"keyDot", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2,"keyDot2", KeyboardButtonListener);

    //--------SpecialCharacters
    eyegui::registerButtonListener(pLayout2, "keySpecial", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keySpecial2", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keySpecial3", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keySpecial4", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keySpecial5", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keySpecial6", KeyboardButtonListener);

    eyegui::registerButtonListener(pLayout2, "keyAe", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyae", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyOe", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyoe", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyUe", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyue", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keySz", KeyboardButtonListener);
    //-------------Symbols ------------------------

    eyegui::registerButtonListener(pLayout2, "keySymbols", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keySymbols2", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keySymbols3", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keySymbols4", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keySymbols5", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keySymbols6", KeyboardButtonListener);

    eyegui::registerButtonListener(pLayout2, "keyCircumflex", KeyboardButtonListener);
    //eyegui::registerButtonListener(pLayout2, "keyCircle", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyExclammark", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyQuotationMark", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyParagraph", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyDollar", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyPercentage", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyAmpersand", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyForwardSlash", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyCurlyBracketOpen", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyRoundBracketOpen", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyRectangularBracketOpen", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyRoundBracketClosing", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyRectangularBracketClosing", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyEquals", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyCurlyBracketClosing", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyQuestionmark", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyBackslash", KeyboardButtonListener);
    //eyegui::registerButtonListener(pLayout2, "keyAccentgrave", KeyboardButtonListener);
    //eyegui::registerButtonListener(pLayout2, "keyAccentaguie", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyStar", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyPlus", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyTilde", KeyboardButtonListener);
    //eyegui::registerButtonListener(pLayout2, "keySingleQuotation", KeyboardButtonListener);
    //eyegui::registerButtonListener(pLayout2, "keyHashtag2", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyUnderscore", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyMinus", KeyboardButtonListener);
    //eyegui::registerButtonListener(pLayout2, "keyDot2", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyColon", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keySemicolon", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyComma", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyPipe", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keySmallerThan", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyGreaterThan", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyAt", KeyboardButtonListener);
    //eyegui::registerButtonListener(pLayout2, "keyEuro", KeyboardButtonListener);

    //--------------Functions---------------
    eyegui::registerButtonListener(pLayout2, "keyFunctions", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyFunctions2", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyFunctions3", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyFunctions4", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyFunctions5", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyFunctions6", KeyboardButtonListener);

    eyegui::registerButtonListener(pLayout2, "keyCopy", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyPaste", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyAddToDict", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyDeleteFromDict", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyClear", KeyboardButtonListener);

    eyegui::registerButtonListener(pLayout2, "keyEnglish", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyGerman", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyFrensh" , KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyDutch", KeyboardButtonListener);

    //--------------Numbers ----------------
    eyegui::registerButtonListener(pLayout2, "keyNumbers", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyNumbers2", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyNumbers3", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyNumbers4", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyNumbers5", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyNumbers6", KeyboardButtonListener);

    eyegui::registerButtonListener(pLayout2, "keyOne", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyTwo", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyThree", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyFour", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyFive", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyZero", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keySix", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keySeven", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyEight", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyNine", KeyboardButtonListener);

    //-------------------------------------
    // Extra Keys
    eyegui::registerButtonListener(pLayout2, "keyFaster", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyFaster3", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyFaster4", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyFaster5", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyFaster6", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keySlower", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keySlower3", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keySlower4", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keySlower5", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keySlower6", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyFaster2", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keySlower2", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyMoveLeft", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyMoveLeft3", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyMoveLeft4", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyMoveLeft5", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyMoveLeft6", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyMoveRight", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyMoveRight3", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyMoveRight4", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyMoveRight5", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyMoveRight6", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyMoveLeft2", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyMoveRight2", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "yes", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "no", KeyboardButtonListener);

    //--------Numbers -----------

    // WordComplition buttons
    eyegui::registerButtonListener(pLayout2, "keyWleft", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyWright", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyWord1", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyWord2", KeyboardButtonListener);
    eyegui::registerButtonListener(pLayout2, "keyWord3", KeyboardButtonListener);
}
