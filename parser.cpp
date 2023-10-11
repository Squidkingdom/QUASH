#include "parser.h"
 
int main() {
    //split the string into tokens by white space
    //https://www.geeksforgeeks.org/infix-to-prefix-conversion-using-two-stacks/
    string TSTCMD = "";
    cout << "Enter a unix command: ";
    getline(cin, TSTCMD);
    vector<string> tokens = split(TSTCMD, ' ');
    string temp = "";
    stack<char> operators;
    string operands;
    string cmd = "";
    for (int i = 0; i < tokens.size(); i++) {
        if (tokens[i] == "<" || tokens[i] == "|" || tokens[i] == ">" || tokens[i] == "&" || tokens[i] == " >>"){
            operands += (temp + " ");
            temp = "";
            operators.push(tokens[i][0]);
        }
        else {
            if (temp == ""){
                temp += tokens[i];
            }else {
                temp += " " + tokens[i];
            }
        }
    }
    string output = "";
    while (operators.size() != 0) {
        output += operators.top();
        output += " ";
        operators.pop();
    }
    output += operands;
    cout << output << endl;
}

int precedence(char op) {
    if (op == '<') {
        return 1;
    }
    else if (op == '|') {
        return 2;
    }
    else if (op == '>') {
        return 3;
    }
    else if (op == '&') {
        return 4;
    }
    else {
        return 0;
    }
}

//https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c
vector<string> split (const string &s, char delim) {
    vector<string> result;
    stringstream ss (s);
    string item;

    while (getline (ss, item, delim)) {
        result.push_back (item);
    }

    return result;
}