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
    // Make directory for BG process to write to.
    mkdir("/tmp/Quash", 0777);

    while(run) {
        usleep(2000); //Fixes timing issue with tag printing

        //Get user input
        string userInput = "";
        flush(cout);
        std::cout<<tag();
        std::getline(cin, userInput);

        //Parse user input
        vector<struct Command> lineCmd;
        tokenize(userInput, &lineCmd);

        //Execute user input
        if (lineCmd.size() > 0)
            spawnExecutor(lineCmd);
    }

    return 0;
}