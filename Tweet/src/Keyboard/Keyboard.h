//============================================================================
// Distributed under the Apache License, Version 2.0.
//============================================================================

#pragma once

#include "externals/eyeGUI-development/include/eyeGUI.h"
#include "src/Buttons/KeyboardButton.h"
#include "externals/Trie/trie.h"
#include <vector>
#include <iostream>

class Keyboard {

public:

    Keyboard(eyegui::Layout* pLayout, eyegui::Layout* pLayout2);
    static Keyboard* createInstance(eyegui::Layout* pLayout, eyegui::Layout* pLayout2);

    void init();
    void tweet();
    void write(const char key);
    void write3(std::string key);
    void write2(const char key);
    void write4(std::string key);

    void abort();
    void activate();
    void deleteKey();
    void respond();
    void insertCursorAndDisplayText(int offset);
    void moveCursor(bool direction);

    void changeSpeed(bool accelerate);
    void resetSpeed();

    void setPointer(std::string&);
    void setPointerValue();
    std::string *pointer;

    int getCas();
    void setCas(int);
    std::string getId();
    void setId(std::string tweetid);

    void UpperKeys();
    void LowerKeys();
    void YesNoKeys();
    void numberKeys();
    void specialKeysFunc();
    void symbolKeys();
    void Listener();

    //More functions for user
    void addTexttoClipboard();
    void addClipboardtoOut();
    void addLinetoDict();
    void deleteLineinDict();
    void clearInput();
    void changeDict(int);

    static Keyboard* getInstance();
    std::string ausgabe, clipboard;

    int currentCursorPos;

    //WordCompletion
    //Default Constructur for Wordcompletion trie-tree
    void showWordComp();
    void WordmoveRight();
    void WordmoveLeft();
    void writeWordComp(int);
    void deleteKey2();
    bool useWordComp=true;
    std::string replaceChar(std::string);
    std::string replaceChar2(std::string);
    std::string replaceChar3(std::string);
    void functionKeys();
    int Wcount;
    std::string  tempWord, word1, word2, word3,dict;
    std::vector<std::string> v;
    Trie trie = Trie("");

    // Test login
    void setPLayout(eyegui::Layout* newLayout) { this->pLayout = newLayout; }

private:

    float WIDTH = 1.0f;
    float HEIGHT = 0.7f;
    float PosX = 0.0f;
    float PosY = 0.3f;
    float speed = 1.0f;
    bool isUpper = true;
    static Keyboard* instance;
    eyegui::Layout* pLayout;
    eyegui::Layout* pLayout2;
    int keys, keys2, keys3, move, Word1, Word2, Word3, cas, numbers, specialKeys, keysSymbol, functionSymbol;
    std::string tweetid;
    std::shared_ptr<KeyboardButton> KeyboardButtonListener = std::shared_ptr<KeyboardButton>(new KeyboardButton);
};
