#include <iostream>
#include <unistd.h>
#include <iostream>

#include <iostream>
#include <unistd.h>
#include <vector>
#include <cstring>
#include <sys/wait.h>
#include <cstdlib>
#include "parser.h"
#include "command.h"


void setupIPC(vector<struct Command>* lineCmd) {
    size_t nCmds = lineCmd->size();
    vector<pid_t> pid(nCmds);
    vector<int[2]> fd(nCmds-1);

    for (int i=0; i<nCmds - 1; i++) {
        pipe(fd[i]);
    }


    for (int i=0; i<nCmds; i++) {
        pid[i] = fork();

        
        if (pid[i] == 0){
            //do child stuff
            struct Command cmd = lineCmd->at(i);

            //pipe to next process
            if (HAS_PIPE){
                dup2(fd[i-1][0], STDIN_FILENO);
                // close(fd[i-1][1]);
                close(fd[i-1][0]);
            }
            if (WILL_PIPE){
                dup2(fd[i][1], STDOUT_FILENO); //dup write end
                close(fd[i][1]); //close write end
                // close(fd[i][0]); //close read end
                // execvp()
            }

            if (cmd.hasRead) {
                int fdin = 0;
                fdin = open(cmd.readFrom.c_str(), O_RDONLY);
                //error check file open
                if (fdin == -1) {
                    std::cerr << "Error: Cannot open file: " << cmd.readFrom << std::endl;
                    exit(1);
                }
                dup2(fdin, STDIN_FILENO);
                close(fdin);
            }
            if(cmd.hasRedirect) {
                int fdout = 0;
                fdout = open(cmd.redirectTo.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0777);
                dup2(fdout, STDOUT_FILENO);
                //execute
                close(fdout);
            }
            if (cmd.redirectAppend) {
                int fdout = 0;
                fdout = open(cmd.redirectTo.c_str(), O_CREAT | O_WRONLY | O_APPEND, 0777);
                dup2(fdout, STDOUT_FILENO);
                //execute
                close(fdout);
            }
            execute(cmd);
        }
        else {
            struct Command cmd = lineCmd->at(i);
            wait(NULL);
            for (auto fd : fd) {
                close(fd[0]);
                close(fd[1]);
            }
        }
    }
}

void echo(std::vector<std::string> args){
    for(int i = 1; i < args.size(); i++){
        std::cout << args[i] << " ";
    }
    std::cout << std::endl;
}

void cd(std::vector<std::string> args){
    if(args.size() == 1){
        chdir(getenv("HOME"));
    }else{
        chdir(args[1].c_str());
    }
}

void pwd(){
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    std::cout << cwd << std::endl;
}

void jobs(){

}

void kill(std::vector<std::string> args){

}
void exportVar(std::vector<std::string> args){
    if(args.size() == 1){
        std::cout << "Error: No arguments given" << std::endl;
    }else{
        std::string var = args[1];
        std::string val = args[2];
        setenv(var.c_str(), val.c_str(), 1);
    }
}
void execute(struct Command cmd){
    std::string bin = cmd.args->at(0);
    if (bin == "echo") {
        echo(*cmd.args);
    }
    else if (bin == "cd") {
        cd(*cmd.args);
    }
    else if (bin == "pwd") {
        pwd();
    }
    else if (bin == "exit" || bin == "quit") {
        exit(0);
    }
    else if (bin == "jobs") {
        jobs();
    }
    else if (bin == "kill") {
        kill(*cmd.args);
    }else if(bin == "export"){
        exportVar(*cmd.args);
    }
    else {
        char **args = new char*[cmd.args->size() + 1];
        for (int i = 0; i < cmd.args->size(); i++) {
            args[i] = const_cast<char*>(cmd.args->at(i).c_str());
        }
        args[cmd.args->size()] = NULL;
        execvp(bin.c_str(), args);
        std::cerr << "Error: Command not found: " << bin << std::endl;
        exit(1);
    }

}