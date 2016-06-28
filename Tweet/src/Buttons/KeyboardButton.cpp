//============================================================================
// Distributed under the Apache License, Version 2.0.
//============================================================================

#include "KeyboardButton.h"
#include "src/Keyboard/Keyboard.h"

/**
* Keyboard button hit function
* When a Button is hit, different functions of the Keyboardclass will be used
* @param[in] pLayout current layout file of GUI
* @param[in] pLayout2 current layout of the Keyboard
* @param[in] id is the ID of the hit Button
*/
void KeyboardButton::hit(eyegui::Layout* pLayout2, std::string id) {

    //UpperCaseLetters
    if (id.compare("keyA") == 0) {
        Keyboard::getInstance()->write3("A");
    }
    if (id.compare("keyB") == 0) {
        Keyboard::getInstance()->write3("B");
    }
    if (id.compare("keyC") == 0) {
        Keyboard::getInstance()->write3("C");
    }
    if (id.compare("keyD") == 0) {
        Keyboard::getInstance()->write3("D");
    }
    if (id.compare("keyE") == 0) {
        Keyboard::getInstance()->write3("E");
    }
    if (id.compare("keyF") == 0) {
        Keyboard::getInstance()->write3("F");
    }
    if (id.compare("keyG") == 0) {
        Keyboard::getInstance()->write3("G");
    }
    if (id.compare("keyH") == 0) {
        Keyboard::getInstance()->write3("H");
    }
    if (id.compare("keyI") == 0) {
        Keyboard::getInstance()->write3("I");
    }
    if (id.compare("keyJ") == 0) {
        Keyboard::getInstance()->write3("J");
    }
    if (id.compare("keyK") == 0) {
        Keyboard::getInstance()->write3("K");
    }
    if (id.compare("keyL") == 0) {
        Keyboard::getInstance()->write3("L");
    }
    if (id.compare("keyM") == 0) {
        Keyboard::getInstance()->write3("M");
    }
    if (id.compare("keyN") == 0) {
        Keyboard::getInstance()->write3("N");
    }
    if (id.compare("keyO") == 0) {
        Keyboard::getInstance()->write3("O");
    }
    if (id.compare("keyP") == 0) {
        Keyboard::getInstance()->write3("P");
    }
    if (id.compare("keyQ") == 0) {
        Keyboard::getInstance()->write3("Q");
    }
    if (id.compare("keyR") == 0) {
        Keyboard::getInstance()->write3("R");
    }
    if (id.compare("keyS") == 0) {
        Keyboard::getInstance()->write3("S");
    }
    if (id.compare("keyT") == 0) {
        Keyboard::getInstance()->write3("T");
    }
    if (id.compare("keyU") == 0) {
        Keyboard::getInstance()->write3("U");
    }
    if (id.compare("keyV") == 0) {
        Keyboard::getInstance()->write3("V");
    }
    if (id.compare("keyW") == 0) {
        Keyboard::getInstance()->write3("W");
    }
    if (id.compare("keyX") == 0) {
        Keyboard::getInstance()->write3("X");
    }
    if (id.compare("keyY") == 0) {
        Keyboard::getInstance()->write3("Y");
    }
    if (id.compare("keyZ") == 0) {
        Keyboard::getInstance()->write3("Z");
    }
    if (id.compare("keyLower") == 0 || id.compare("keyNumbers3")==0 || id.compare("keySpecial4")==0 || id.compare("keySymbols5")==0 || id.compare("keyFunctions6")==0) {
        Keyboard::getInstance()->LowerKeys();
    }

    //LowerCaseLetters
    if (id.compare("keya") == 0) {
        Keyboard::getInstance()->write3("a");
    }
    if (id.compare("keyb") == 0) {
        Keyboard::getInstance()->write3("b");
    }
    if (id.compare("keyc") == 0) {
        Keyboard::getInstance()->write3("c");
    }
    if (id.compare("keyd") == 0) {
        Keyboard::getInstance()->write3("d");
    }
    if (id.compare("keye") == 0) {
        Keyboard::getInstance()->write3("e");
    }
    if (id.compare("keyf") == 0) {
        Keyboard::getInstance()->write3("f");
    }
    if (id.compare("keyg") == 0) {
        Keyboard::getInstance()->write3("g");
    }
    if (id.compare("keyh") == 0) {
        Keyboard::getInstance()->write3("h");
    }
    if (id.compare("keyi") == 0) {
        Keyboard::getInstance()->write3("i");
    }
    if (id.compare("keyj") == 0) {
        Keyboard::getInstance()->write3("j");
    }
    if (id.compare("keyk") == 0) {
        Keyboard::getInstance()->write3("k");
    }
    if (id.compare("keyl") == 0) {
        Keyboard::getInstance()->write3("l");
    }
    if (id.compare("keym") == 0) {
        Keyboard::getInstance()->write3("m");
    }
    if (id.compare("keyn") == 0) {
        Keyboard::getInstance()->write3("n");
    }
    if (id.compare("keyo") == 0) {
        Keyboard::getInstance()->write3("o");
    }
    if (id.compare("keyp") == 0) {
        Keyboard::getInstance()->write3("p");
    }
    if (id.compare("keyq") == 0) {
        Keyboard::getInstance()->write3("q");
    }
    if (id.compare("keyr") == 0) {
        Keyboard::getInstance()->write3("r");
    }
    if (id.compare("keys") == 0) {
        Keyboard::getInstance()->write3("s");
    }
    if (id.compare("keyt") == 0) {
        Keyboard::getInstance()->write3("t");
    }
    if (id.compare("keyu") == 0) {
        Keyboard::getInstance()->write3("u");
    }
    if (id.compare("keyv") == 0) {
        Keyboard::getInstance()->write3("v");
    }
    if (id.compare("keyw") == 0) {
        Keyboard::getInstance()->write3("w");
    }
    if (id.compare("keyx") == 0) {
        Keyboard::getInstance()->write3("x");
    }
    if (id.compare("keyy") == 0) {
        Keyboard::getInstance()->write3("y");
    }
    if (id.compare("keyz") == 0) {
        Keyboard::getInstance()->write3("z");
    }
    if (id.compare("keyUpper") == 0)  {
        Keyboard::getInstance()->UpperKeys();
    }
    // Special Characters

    if(id.compare("keyHashtag") ==0) {
          Keyboard::getInstance() -> write3("#");
    }

    //Punctuation

    if(id.compare("keyDot") == 0 || id.compare("keyDot2") ==0 ){
          Keyboard::getInstance()->write3(".");
    }
    //Yes and NO Keys
    if (id.compare("yes") == 0) {
        int cas = Keyboard::getInstance()->getCas();
        if (cas == 1) {
            std::cout << "Tweete in Twitter" << std::endl;
            Keyboard::getInstance()->tweet();
        }
        if (cas == 2) {
            std::cout << "Antworte auf Tweet" << std::endl;
            Keyboard::getInstance()->respond();
        }
        if (cas == 3) {
            std::cout << "Gebe Text weiter an Pointer" << std::endl;
            Keyboard::getInstance()->setPointerValue();
        }
    }


    if (id.compare("no") == 0) {
        Keyboard::getInstance()->UpperKeys();;
    }
  //-------------------------Numbers -------------------------
    if(id.compare("keyZero") ==0 ) {
      Keyboard::getInstance() -> write3("0");
    }
    if(id.compare("keyOne") ==0 ) {
      Keyboard::getInstance() -> write3("1");
    }
    if(id.compare("keyTwo") ==0 ) {
      Keyboard::getInstance() -> write3("2");
    }
    if(id.compare("keyThree") ==0 ) {
      Keyboard::getInstance() -> write3("3");
    }
    if(id.compare("keyFour") ==0 ) {
      Keyboard::getInstance() -> write3("4");
    }
    if(id.compare("keyFive") ==0 ) {
      Keyboard::getInstance() -> write3("5");
    }
    if(id.compare("keySix") ==0 ) {
      Keyboard::getInstance() -> write3("6");
    }
    if(id.compare("keySeven") ==0 ) {
      Keyboard::getInstance() -> write3("7");
    }
    if(id.compare("keyEight") ==0 ) {
      Keyboard::getInstance() -> write3("8");
    }
    if(id.compare("keyNine") ==0 ) {
      Keyboard::getInstance() -> write3("9");
    }

    //-------------Symbols -------------------------------

    if(id.compare("keyCircumflex") ==0 ) {
      Keyboard::getInstance() -> write3("^");
    }
    if(id.compare("keyCircle") ==0 ) {
      Keyboard::getInstance() -> write3("°");
    }
    if(id.compare("keyExclammark") ==0 ) {
      Keyboard::getInstance() -> write3("!");
    }
    if(id.compare("keyQuotationMark") ==0 ) {
      Keyboard::getInstance() -> write3("\"");
    }
    if(id.compare("keyParagraph") ==0 ) {
      Keyboard::getInstance() -> write3("§");
    }
    if(id.compare("keyDollar") ==0 ) {
      Keyboard::getInstance() -> write3("$");
    }
    if(id.compare("keyPercentage") ==0 ) {
      Keyboard::getInstance() -> write3("%");
    }
    if(id.compare("keyAmpersand") ==0 ) {
      Keyboard::getInstance() -> write3("&");
    }
    if(id.compare("keyForwardSlash") ==0 ) {
      Keyboard::getInstance() -> write3("/");
    }
    if(id.compare("keyCurlyBracketOpen") ==0 ) {
      Keyboard::getInstance() -> write3("{");
    }
    if(id.compare("keyRoundBracketOpen") ==0 ) {
      Keyboard::getInstance() -> write3("(");
    }
    if(id.compare("keyRectangularBracketOpen") ==0 ) {
      Keyboard::getInstance() -> write3("[");
    }
    if(id.compare("keyRoundBracketClosing") ==0 ) {
      Keyboard::getInstance() -> write3(")");
    }
    if(id.compare("keyRectangularBracketClosing") ==0 ) {
      Keyboard::getInstance() -> write3("]");
    }
    if(id.compare("keyEquals") ==0 ) {
      Keyboard::getInstance() -> write3("=");
    }
    if(id.compare("keyCurlyBracketClosing") ==0 ) {
      Keyboard::getInstance() -> write3("}");
    }
    if(id.compare("keyQuestionmark") ==0 ) {
      Keyboard::getInstance() -> write3("?");
    }
    if(id.compare("keyBackslash") ==0 ) {
      Keyboard::getInstance() -> write3("\\");
    }
    if(id.compare("keyAccentgrave") ==0 ) {
      Keyboard::getInstance() -> write3("`");
    }
    if(id.compare("keyAccentaguie") ==0 ) {
      Keyboard::getInstance() -> write3(" \'");
    }
    if(id.compare("keyStar") ==0 ) {
      Keyboard::getInstance() -> write3("*");
    }
    if(id.compare("keyPlus") ==0 ) {
      Keyboard::getInstance() -> write3("+");
    }
    if(id.compare("keyTilde") ==0 ) {
      Keyboard::getInstance() -> write3("~");
    }
    if(id.compare("keySingleQuotation") ==0 ) {
      Keyboard::getInstance() -> write3("\'");
    }
    if(id.compare("keyHashtag2") ==0 ) {
      Keyboard::getInstance() -> write3("#");
    }
    if(id.compare("keyUnderscore") ==0 ) {
      Keyboard::getInstance() -> write3("_");
    }
    if(id.compare("keyMinus") ==0 ) {
      Keyboard::getInstance() -> write3("-");
    }
    if(id.compare("keyDot2") ==0 ) {
      Keyboard::getInstance() -> write3(".");
    }
    if(id.compare("keyColon") ==0 ) {
      Keyboard::getInstance() -> write3(":");
    }
    if(id.compare("keySemicolon") ==0 ) {
      Keyboard::getInstance() -> write3(";");
    }
    if(id.compare("keyComma") ==0 ) {
      Keyboard::getInstance() -> write3(",");
    }
    if(id.compare("keyPipe") ==0 ) {
      Keyboard::getInstance() -> write3("|");
    }
    if(id.compare("keySmallerThan") ==0 ) {
      Keyboard::getInstance() -> write3("<");
    }
    if(id.compare("keyGreaterThan") ==0 ) {
      Keyboard::getInstance() -> write3(">");
    }
    if(id.compare("keyAt") ==0 ) {
      Keyboard::getInstance() -> write3("@");
    }
    if(id.compare("keyEuro") ==0 ) {
      Keyboard::getInstance() -> write3("€");
    }
    //---------------------------Special Characters --------------
    if(id.compare("keyAe") ==0 ) {
      Keyboard::getInstance() -> write3("Ä");
    }
    if(id.compare("keyae") ==0 ) {
      Keyboard::getInstance() -> write3("ä");
    }
    if(id.compare("keyOe") ==0 ) {
      Keyboard::getInstance() -> write3("Ö");
    }
    if(id.compare("keyoe") ==0 ) {
      Keyboard::getInstance() -> write3("ö");
    }
    if(id.compare("keyUe") ==0 ) {
      Keyboard::getInstance() -> write3(u8"Ü");
    }
    if(id.compare("keyue") ==0 ) {
      Keyboard::getInstance() -> write3(u8"ü");
    }
    if(id.compare("keySz") ==0 ) {
        Keyboard::getInstance() -> write3(u8"ß");
    }
    //-----------------------Functions----------------------------------------
    if(id.compare("keyCopy") ==0 ) {
     Keyboard::getInstance()->addTexttoClipboard();
    }

    if(id.compare("keyPaste") ==0 ) {
      Keyboard::getInstance()->addClipboardtoOut();
    }
    if(id.compare("keyAddToDict") ==0 ) {
     Keyboard::getInstance()->addLinetoDict();
    }
    if(id.compare("keyDeleteFromDict") ==0 ) {
      Keyboard::getInstance()->deleteLineinDict();
    }
    if(id.compare("keyClear") ==0 ) {
      Keyboard::getInstance()->clearInput();
    }

    if(id.compare("keyEnglish") == 0){
      Keyboard::getInstance()->changeDict(0);
    }
    if(id.compare("keyGerman") == 0){
      Keyboard::getInstance()->changeDict(1);
    }
    if(id.compare("keyFrensh") == 0){
      Keyboard::getInstance()->changeDict(2);
    }
    if(id.compare("keyDutch") == 0){
      Keyboard::getInstance()->changeDict(3);
    }


    //-------------------------------------------------------------------------
        //WordCompletion Keys
    if (id.compare("keyWright") == 0) {
        Keyboard::getInstance()->WordmoveRight();
    }
    if (id.compare("keyWleft") == 0) {
        Keyboard::getInstance()->WordmoveLeft();
    }
    if (id.compare("keyWord1") == 0) {
        Keyboard::getInstance()->writeWordComp(1);
    }
    if (id.compare("keyWord2") == 0) {
        Keyboard::getInstance()->writeWordComp(2);
    }
    if (id.compare("keyWord3") == 0) {
        std::cout << "Schreibe drittes Wort " << std::endl;
        Keyboard::getInstance()->writeWordComp(3);
    }
    //-------------------------------------------------------------------------

    //OtherKes
    if (id.compare("keyDelete") == 0 || id.compare("keyDelete2") == 0 || id.compare("keyDelete3") == 0 || id.compare("keyDelete4")==0 || id.compare("keyDelete5") == 0 || id.compare("keyDelete6")==0) {
        Keyboard::getInstance()->deleteKey();
    }
    if (id.compare("keyLeer") == 0 || id.compare("keyLeer2") == 0 || id.compare("keyLeer3") == 0 || id.compare("keyLeer4")==0 || id.compare("keyLeer5") == 0 || id.compare("keyLeer6")==0) {
        Keyboard::getInstance()->write3(" ");
    }
    if (id.compare("keyAbort") == 0) {
        std::cout << "Closing Keyboard Layout" << std::endl;
        Keyboard::getInstance()->abort();
    }

    if (id.compare("keyEnter") == 0 || id.compare("keyEnter2") == 0 || id.compare("keyEnter3") == 0 || id.compare("keyEnter4")==0 || id.compare("keyEnter5") == 0 || id.compare("keyEnter6")==0) {

        int cas = Keyboard::getInstance()->getCas();
        if (cas == 1) {
            std::cout << "Tweete in Twitter" << std::endl;
            Keyboard::getInstance()->tweet();
        }
        if (cas == 2) {
            std::cout << "Antworte auf Tweet" << std::endl;
            Keyboard::getInstance()->respond();
        }
        if (cas == 3) {
            std::cout << "Gebe Text weiter an Pointer" << std::endl;
            Keyboard::getInstance()->setPointerValue();
        }

    }

    if (id.compare("keyMoveRight2") == 0 || id.compare("keyMoveRight") == 0 || id.compare("keyMoveRight3") == 0 || id.compare("keyMoveRight4")==0 || id.compare("keyMoveRight5") == 0 || id.compare("keyMoveRight6")==0) {
        Keyboard::getInstance()->moveCursor(true);
    }
    if (id.compare("keyMoveLeft2") == 0 || id.compare("keyMoveLeft") == 0 || id.compare("keyMoveLeft3")==0|| id.compare("keyMoveLeft4")==0 || id.compare("keyMoveLeft5") == 0 || id.compare("keyMoveLeft6")==0){
        Keyboard::getInstance()->moveCursor(false);
    }

    if (id.compare("keyFaster") == 0 || id.compare("keyFaster2") == 0 || id.compare("keyFaster3") == 0 || id.compare("keyFaster4")==0 || id.compare("keyFaster5") == 0 || id.compare("keyFaster6")==0) {
        std::cout << "key Faster an" << std::endl;
        Keyboard::getInstance()->changeSpeed(true);
    }
    if (id.compare("keySlower") == 0 || id.compare("keySlower2") == 0 || id.compare("keySlower3") == 0 || id.compare("keySlower4")==0 || id.compare("keySlower5") == 0 || id.compare("keySlower6")==0) {
        std::cout << "key slower an" << std::endl;
    Keyboard::getInstance()->changeSpeed(false);
    }

    if(id.compare("keyNumbers") == 0 || id.compare("keyNumbers2")==0 || id.compare("keyNumbers4")==0 || id.compare("keyNumbers5") == 0 || id.compare("keyNumbers6")==0){
      Keyboard::getInstance()->numberKeys();
    }
    if(id.compare("keySpecial") ==0 || id.compare("keySpecial2") == 0 || id.compare("keySpecial3") == 0 ||  id.compare("keySpecial5") == 0 || id.compare("keySpecial6")==0){
      Keyboard::getInstance()->specialKeysFunc();
    }
    if(id.compare("keyFunctions") ==0 || id.compare("keyFunctions2") == 0 || id.compare("keyFunctions3") == 0 || id.compare("keyFunctions4")==0 || id.compare("keyFunctions5") == 0 ){
      Keyboard::getInstance()->functionKeys();
    }
    if(id.compare("keySymbols") ==0 || id.compare("keySymbols2") == 0 || id.compare("keySymbols3") == 0 || id.compare("keySymbols4")==0 || id.compare("keySymbols6")==0){
      Keyboard::getInstance()->symbolKeys();
    }
}

/**
* Keyboard button down function
* not in use
* Button use different functions of the Keyboardclass
* @param[in] pLayout current layout file of GUI
* @param[in] pLayout2 current layout of the Keyboard
* @param[in] id is the ID of the hit Button
*/
void KeyboardButton::down(eyegui::Layout* pLayout2, std::string id) {

}

/**
* Keyboard button up function
* not in use
* Button use different functions of the Keyboardclass
* @param[in] pLayout current layout file of GUI
* @param[in] pLayout2 current layout of the Keyboard
* @param[in] id is the ID of the hit Button
*/
void KeyboardButton::up(eyegui::Layout* pLayout2, std::string id) {

//	std::cout << id << ":up, KeyboardButton" << std::endl;

}
