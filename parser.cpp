#include "parser.h"
 
// int main() {
//     //split the string into tokens by white space
//     //https://www.geeksforgeeks.org/infix-to-prefix-conversion-using-two-stacks/
//     string TSTCMD = "cat < files.txt | grep '*.c' > out.txt &";
//     struct Command* cmd = new Command;
//     tokenize(TSTCMD, cmd);
//     printf("Tokens: \n");
    
// }

void tokenize (const string &s, struct Command* cmd) {
    stringstream ss (s);
    string item;

    while (getline (ss, item, '\n')) {
        for (int i = 0; i < item.length(); i++) {
            if (item[i] == '&') {
                cmd->isBackground = true;
                item.erase(i, 1);
                i=-1;
                continue;
            }
            if (item[i] == '|') {
                cmd->hasPipe = true;
                continue;
            }
            if (item[i] == '<') {
                cmd->hasRead = true;
                continue;
            }
            if (item[i] == '>') {
                cmd->hasRedirect = true;
                continue;
            }
            if (item[i] == '\'' || item[i] == '\"') {
                for (int j = (i+1); j < item.length(); j++) {
                    if (item[j] == '\'' || item[j] == '\"') {
                        char* temp = (char*)malloc(sizeof(char)*j-1);
                        item.copy(temp, j-1, 1);
                        cmd->tokens->push_back (temp);
                        item.erase(0, j+1);
                        i=-1;
                        break;
                    }
                }
                continue;
            }
            if (item[i] == ' ') {
                char* temp = (char*)malloc(sizeof(char)*i);
                item.copy (temp, i, 0);
                item.erase(0, i+1);
                if (i > 0)
                    cmd->tokens->push_back(temp);
                delete temp;
                i=-1;

            }
        }
    }
}