#include "trie.h"

#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <set>

#include <fstream>
#include <iostream>

using namespace std;

/**
* Constructor of the Tree Class
* Keyboard uses it for the Wordcomplition
* @param[in] first Node must be empty
*/
Trie::Trie(const string &val = "") {
    value = val;
    flag = false;
}

/**
* Function of the Tree Class
* searches after a word in the Trie-Tree
* @param[in] word variable used for the search
*/
string Trie::find(const string &word) {
    Trie * node = this;
    for (size_t i = 0; i < word.length(); i++) {
        const char c = word[i];
        if (node->children.find(c) == node->children.end())
            return "";
        else
            node = &node->children[c];
    }
    return node->value;
}

//------------------------------------------------------------------
/**
* Function of the Tree Class
* Adds Word to the Trie-Tree
* @param[in] word variable used for the insert
*/
void Trie::insert(const string &word) {
    Trie * node = this;
    for (size_t i = 0; i < word.length(); i++) {
        const char c = word[i];
        if (node->children.find(c) == node->children.end())
            node->add(c);
        node = &node->children[c];
    }
    node->flag = true;
}

/**
* Function of the Tree Class
* Adds Char to the Trie-Tree
* used by the insert function
* @param[in] Char variable used for the insert
*/
void Trie::add(char c) {
    if (value == "")
        children[c] = Trie(string(1, c));
    else
        children[c] = Trie(value + c);
}
//------------------------------------------------------------------

/**
* Function of the Tree Class
* searches all Prefixes
*/
vector<string> Trie::all_prefixes() {
    vector<string> results;
    if (flag)
        results.push_back(value);

    if (children.size()) {
        map<char, Trie>::iterator iter;
        vector<string>::iterator node;
        for (iter = children.begin(); iter != children.end(); iter++) {
            vector<string> nodes = iter->second.all_prefixes();
            results.insert(results.end(), nodes.begin(), nodes.end());
        }
    }
    return results;
}

/**
* Function of the Tree Class
* Seaches Trie-Tree for words on hand of the prefix
* @param[in] prefix variable used for the Search
*/
vector<string> Trie::autocomplete(const string &prefix) {
    Trie * node = this;
    vector<string> results;
    for (size_t i = 0; i < prefix.length(); i++) {
        const char c = prefix[i];
        if (node->children.find(c) == node->children.end())
            return results;
        else
            node = &node->children[c];
    }
    return node->all_prefixes();
}


/*
TODO
--Save Trie-Tree to file (if possible)
--load Trie-Tree from file (if possible)
*/

/**
* Function of the Tree Class
* Load Dictionary from File
* @param[in] dict variable is the Dictionary-Path
*/
void Trie::loadDict(std::string dict) {
    Trie * trie = this;

    std::ifstream inf;
    std::string word;

    std::cout << "Keyboard: Opening Dictionary: " << dict << std::endl;
    //inf.open("dict.txt");
    inf.open(dict);

    if (!inf.is_open() || inf.fail())
        cout << "Error opening Dictionary file - quit\n";
    while (getline(inf, word)) {
        trie->insert(word);
        if (inf.bad())
            perror("error while reading file");
    }
    inf.clear(); // clear eof and fail bits
    inf.seekg(0, ios::beg);
    inf.close();
}

/**
* Function of the Tree Class
* Load Dictionary from File
* @param[in] dict variable is the Dictionary-Path
* @param[in] line variable is the String added to the Dictionary
*/
void Trie::addLinetoDict(std::string dict, std::string line) {
    std::ofstream out(dict, std::ios::app);
    out << line + "\n";
}

/**
* Function of the Tree Class
* Load Dictionary from File
* @param[in] dict variable is the Dictionary-Path
* @param[in] line variable is the String which should be deleted in the Dictionary
*/
void Trie::deleteLineinDict(std::string dict, std::string line) {

    ifstream dictionary(dict);
    ofstream temp("temp.txt"); // temp file for input of every student except the one user wants to delete
    std::string word;

    while (getline(dictionary, word)) {

        if (word != line) {
            temp << word + "\n";
        }
        else {
            std::cout << "Word == " + word + " == was deleted" << std::endl;
        }

    }

    dictionary.clear(); // clear eof and fail bits
    dictionary.seekg(0, ios::beg);
    dictionary.close();
    temp.close();


    const char * c = dict.c_str();

    remove(c);
    rename("temp.txt", c);
}
