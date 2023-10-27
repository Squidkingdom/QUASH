#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <termios.h>
#include <string>
#include <vector>
#include <stack>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include "command.h"
#include "parser.h"

using namespace std;




int main() {
    bool run = true;
    mkdir("/tmp/Quash", 0777);
    while(run) {
        std::string userInput = "";
        std::cout<<"[QUASH]$ ";
        std::getline(std::cin, userInput);
        vector<struct Command>* lineCmd;
        tokenize(userInput, lineCmd);
        setupIPC(lineCmd);


    }

    return 0;
}