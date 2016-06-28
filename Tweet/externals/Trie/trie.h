/*
* Trie data structure using STL
*
* Author : Vivek Narayanan

Modified for "GazeTheWeb - Tweet" application (01/01/2016)
*/

#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <set>

using namespace std;

class Trie {
public:
    map<char, Trie> children;
    string value;
    bool flag;

    Trie(const string &);
    void add(char);
    string find(const string &);
    void insert(const string &);
    vector<string> all_prefixes();
    vector<string> autocomplete(const string &);
    void loadDict(std::string);
    void addLinetoDict(std::string, std::string);
    void deleteLineinDict(std::string, std::string);
};
