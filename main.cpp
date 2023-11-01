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
#include "executive.h"
#include "parser.h"
#include "built-ins.h"

using namespace std;



//TODO remove from jobs


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
        // {[cat, a.txt], [grep, -o, World, >>, b.txt]}
        // cat a.txt | "grep -o" World > b.txt  
        // lineCmd is comprised of subCmds in the form of `Stuct Command`
        vector<struct Command> lineCmd;
        tokenize(userInput, &lineCmd);

        //Execute user input
        if (lineCmd.size() > 0)
            spawnExecutor(lineCmd);

        
    }

    return 0;
}