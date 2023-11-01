#ifndef PARSER_H
#define PARSER_H
#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstring>
#include <stack>
#include "executive.h"
#include "parser.h"
using namespace std;
typedef struct Command {
        vector<string> args;
        bool isBackground = false; // unilateral between all subcmds in same linecmd
        bool hasRead = false; // < 
        string readFrom= "";
        bool hasRedirect = false; // > //first subcmd cannot have hasRedirect.
        bool redirectAppend = false; // >>
        string redirectTo = "";
    }Command;

void tokenize (string s, vector<struct Command>* linecmd);
string expandEnvironmentVariables(const string& input);

        //command
        //is background

//create a structure to hold the tokens, and flags
#endif