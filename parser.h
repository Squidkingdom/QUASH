#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <stack>
using namespace std;
typedef struct Command {
        vector<string>* tokens = new vector<string>;
        vector<string>* operations = new vector<string>;
        bool isBackground = false;
        bool hasPipe = false;
        bool hasRead = false;
        bool hasRedirect = false;
}Command;

void tokenize (const string &s, struct Command* cmd);
        //command
        //is background

//create a structure to hold the tokens, and flags
