#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <termios.h>
#include <string>
#include <vector>
#include <stack>
#include <fstream>
#include <sstream>
#include <unistd.h>
using namespace std;

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

void echo(vector<string> tokens) {
    int i = 0;
    string outputStr = "";
    for (i=1; i<tokens.size(); i++) {
        outputStr.append(tokens[i]);
        outputStr.append(" ");
    }
    std::cout<<outputStr;
    std::cout<<"\n";
}

void executeCommand(vector<string> command) {
    int i = 0;
    int j = 0;
    if (command[0] == "echo") {
        echo(command);
    }
}

void do_job(vector<vector<string>> commands, vector<string> operations) {
    // cout<<"made it to job";
    // cout<<operations[0];
    pid_t pid;
    pid = fork();
    cout<<pid;
    if (pid == 0) {
        cout<<"made it further";
        string filename = "";
        if (operations.size() == 0) {
            executeCommand(commands[0]);
        }
        else if (operations[0] == ">") {//need to loop through operations
            filename = commands[1][0];
            int fdout = 0;
            fdout = open(filename.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0777);
            dup2(fdout, STDOUT_FILENO);
            executeCommand(commands[0]);
            // echo(commands[0]);
            close(fdout);
            exit(0);
        } 
        else if (operations[0] == "<") {
            cout<<"should get here?";
            FILE* fp;
            char *input;
            filename = commands[1][0];
            fp = fopen(filename.c_str(), "r");
            if (fp == NULL) {
                cout<<"Error file not found!";
            }
            vector<string> tempVec{};
            while (fgets(input, 1024, fp) != NULL) {
                std::cout<<"problem";
                tempVec = split(input, ' ');
                int i = 0;
                for(i=0;i<tempVec.size();i++) {
                    commands[0].push_back(tempVec[i]);
                }
            }
            std::cout<<commands[0][1];
            executeCommand(commands[0]);
            fclose(fp);
        }
    }
}

vector<vector<string>> parser(std::string userInput) {
    vector<string> tokens = split(userInput, ' ');
    std::cout<<tokens.size();

    vector<vector<string>> commands{};//this is a vector of vectors of strings, each sub vector holds the arguments of one command 
    //for example {{"echo", "hello", "world"}, {"a.txt"}}

    vector<string> operations{};//this holds the things like "<", ">", ">>", and "|"
    int i = 0;
    
    vector<string> tempVec{};
    for(i=0; i<tokens.size(); i++) {
        if (tokens[i] == ">" || tokens[i] == "<" || tokens[i] == ">>" || tokens[i] == "|") {
            commands.push_back(tempVec);
            operations.push_back(tokens[i]);
            tempVec.clear();
        } else {
            tempVec.push_back(tokens[i]);
        }
    }
    commands.push_back(tempVec);
    std::cout<<commands.size();
    std::cout<<commands[0][0];
    std::cout<<"\n";
    // std::cout<<commands[1][0];
    std::cout<<"\n";
    // std::cout<<operations[0];
    std::cout<<"\n";
    do_job(commands, operations);
    return commands;
}

int main() {
    bool run = true;
    while(run) {
        std::string userInput = "";
        std::cout<<"[QUASH]$ ";
        std::getline(std::cin, userInput);
        vector<vector<string>> commands{};
        commands = parser(userInput);
    }

    return 0;
}