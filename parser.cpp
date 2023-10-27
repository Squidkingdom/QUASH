#include "parser.h"
 
// int main() {
//     string TSTCMD = "cat < files.txt | grep '*.c' > out.txt &";
//     vector<struct Command> cmd;
//     tokenize(TSTCMD, &cmd);

//     printf("Tokens: \n");
    
// }


void tokenize (string s, vector<struct Command>* linecmd) {
    bool isBackground = false;
    vector<string> subcmds;
    size_t found = s.find('&');
    if(found != string::npos) {
        isBackground = true;
        s.erase(found, 1);
    }
    subcmds = split(s, '|');

    
    //TODO EnvVar
    for (int k = 0; k < subcmds.size(); k++) {
        auto item = subcmds[k];
        struct Command* cmd = new Command;
        cmd->isBackground = isBackground;
        for (int i = 0; i < item.length(); i++) {
            if (item[i] == '<') {
                cmd->hasRead = true;
                item.erase(0, i+1);
                for (int j = 1; j < item.length(); j++) {
                    if (item[j] == ' ' || j == item.length()-1) {
                        char* temp = (char*)calloc(j, sizeof(char));
                        item.copy(temp, j, 1);
                        cmd->readFrom = temp;
                        item.erase(0, j+1);
                        i=-1;
                        continue;
                    }
                }
            }
            if (item[i] == '>' && k == subcmds.size()-1) {
                if (item[i+1] == '>') {
                    cmd->redirectAppend = true;
                    cmd->redirectTo = trim(split(item, '>')[2], ' ');
                    break;
                }
                cmd->hasRedirect = true;
                cmd->redirectTo = trim(split(item, '>')[1], ' ');
                break;
            }
            if (item[i] == '\'' || item[i] == '\"') {
                for (int j = (i+1); j < item.length(); j++) {
                    if (item[j] == '\'' || item[j] == '\"') {
                        char* temp = (char*)calloc(j-1, sizeof(char));
                        item.copy(temp, j-1, 1);
                        cmd->args->push_back (temp);
                        item.erase(0, j+1);
                        i=-1;
                        break;
                    }
                }
                continue;
            }
            if (item[i] == ' ') {
                char* temp = (char*)calloc(i, sizeof(char));
                item.copy(temp, i, 0);
                item.erase(0, i+1);
                if (i > 0) {
                    string tempStr(temp);
                    if (tempStr.find('$') != string::npos) {
                        cmd->args->push_back(expandEnvironmentVariables(tempStr));
                    }
                    else {
                        cmd->args->push_back(temp);
                    }
                }
                i=-1;
            }
        }
        linecmd->push_back(*cmd);
    }
}

string expandEnvironmentVariables(const string& input) {
    string result = input;
    size_t sPos = 0;

    while((sPos = result.find('$', sPos)) != string::npos) {
        size_t ePos = sPos + 1;
        while(ePos < result.size() && isupper(result[ePos])) {
            ++ePos;
        }
        size_t len = ePos - sPos;
        string envName = result.substr(sPos + 1, len - 1);
        const char* envVal = getenv(envName.c_str());

        if(envVal != nullptr) {
            result.replace(sPos, len, envVal);
            sPos += strlen(envVal);
        } else {
            sPos = ePos;
        }
    }

    return result;
}

vector<string> split (const string &s, char delim) {
    vector<string> result;
    stringstream ss (s);
    string item;

    while (getline (ss, item, delim)) {
        item = trim(item, ' ');
        result.push_back (item);
    }

    return result;
}

string trim(string s, char delim) {
    int i = s.length();
    if (i == 0) {
        return s;
    }
    if (s[0] == delim) {
        s.erase(0, 1);
    }
    if (s[i-1] == delim) {
        s.erase(i-1, 1);
    }
    return s;
}