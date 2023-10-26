#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <stack>
using namespace std;
typedef struct Command {
        vector<string>* args = new vector<string>;
        bool isBackground = false; // unilateral between all subcmds in same linecmd
        bool hasRead = false;
        string readFrom= "";
        bool hasRedirect = false; //first subcmd cannot have hasRedirect.
        bool redirectAppend = false;
        string redirectTo = "";
    }Command;

void tokenize (string s, vector<struct Command>* linecmd);
vector<string> split (const string &s, char delim);
string trim(string s, char delim);
        //command
        //is background

//create a structure to hold the tokens, and flags
