#include <iostream>
#include <unistd.h>
#include <iostream>
#include <filesystem>
#include <iostream>
#include <unistd.h>
#include <vector>
#include <string>
#include <cstring>
#include <sys/wait.h>
#include <sys/stat.h>
#include <cstdlib>
#include "parser.h"
#include "command.h"



struct Process
{
  int id;
  int pid;
  bool isBackground;
  string cmd = "";
};

static int totalJobs;
static int nextId;
static struct Process myJobs[5];
static string fifoDir = "/tmp/Quash/";
string tag(){
    usleep(2000);
    return "[QUASHH]$ ";
}

void handleChildDone(int s){
    pid_t pid;
    int status;
    // printf("handling child done\n");
    while((pid = waitpid(-1, &status, WNOHANG)) > 0){
        // printf("Job with pid %d done\n", pid);
        for(int i = 0; i < totalJobs; i++){
            if(myJobs[i].pid == pid){
                string fifo = fifoDir + "/" + to_string(myJobs[i].id);
                string line;
                ifstream fifoStream(fifo);
            
                while (getline(fifoStream, line)) {
                    cout << line << endl;
                }
                fifoStream.close();
                remove(fifo.c_str());
                std::cout << "[" << myJobs[i].id << "] " << myJobs[i].pid << " Done" << std::endl << tag();
                flush(std::cout);

                myJobs[i].pid = 0;
                myJobs[i].id = 0;
            }
        }
    }
}


void chainExe(vector<struct Command> lineCmd)
{
    
    size_t nCmds = lineCmd.size();
    vector<pid_t> pid(nCmds);
    vector<int[2]> fd(nCmds-1);

    for (int i=0; i< nCmds - 1; i++) {
        pipe(fd[i]);
    }


    for (int i=0; i<nCmds; i++) {
        pid[i] = fork();

        
        if (pid[i] == 0){
            //do child stuff
            struct Command cmd = lineCmd.at(i);

            //pipe to next process
            if (HAS_PIPE){
                dup2(fd[i-1][0], STDIN_FILENO);
                // char buf[1024];
                // int n = read(fd[i-1][0], buf, 1024);
                // printf("read %d bytes\n", n);
                // printf("read: %s\n", buf);
                // printf("has pipe");
            }
            if (WILL_PIPE){
                dup2(fd[i][1], STDOUT_FILENO);
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
            exit(0);
        }
        else {
            //parent code
            struct Command cmd = lineCmd.at(i);
            if (HAS_PIPE){
                close(fd[i-1][0]);
            }
            if (WILL_PIPE){
                close(fd[i][1]);
                //read from the pipe to see if wer sent anything
                
            }
            // printf("Waiting...\n");
            waitpid(pid[i], NULL, 0);
        }
    }
        wait(NULL);
        close(1);
        exit(0);
}

void clearFifo(){
    try {
        for (const auto & entry : std::filesystem::directory_iterator(fifoDir)) {
            std::filesystem::remove(entry.path());
            
        }
    } catch (std::filesystem::filesystem_error& e) {
        std::cerr << e.what() << '\n';
    }
}

void cd(std::vector<std::string> args){
    if(args.size() == 1){
        chdir(getenv("HOME"));
    }else{
        chdir(args[1].c_str());
    }
}

void exportVar(std::vector<std::string> args){
    if(args.size() == 1){
        std::cout << "Error: No arguments given" << std::endl;
    }else{
        vector<string> subArgs = split(args[1], '=');
        // std::string var = args[1];
        // std::string val = args[2];
        setenv(subArgs[0].c_str(), subArgs[1].c_str(), 1);
    }
}

void spawnExecutor(vector<struct Command> lineCmd) {
    pid_t lcPid;
    
    if (lineCmd[0].args->at(0) == "export") {
        exportVar(*lineCmd[0].args);
    }
    if (lineCmd[0].args->at(0) == "exit" || lineCmd[0].args->at(0) == "quit") {
        exit(0);
    }
    if (lineCmd[0].args->at(0) == "cd") {//calling cd at the start because it only changes dir of current process so parent must do it
        cd(*lineCmd[0].args);
    }
    int bg = lineCmd[0].isBackground;
    if (bg) {
        signal(SIGCHLD, handleChildDone);

    }
    
    lcPid = fork();
    if (lcPid == 0) {
        signal(SIGCHLD, SIG_DFL);
        if (bg) {
            string fifo = (fifoDir + to_string(nextId));
            clearFifo();
            //I actually dont make it a fifo, but a file
            //This is because fifos must have an open reader and writer to get the FD
            //I dont want to have to open a reader yet, so I just make it a file
            int fifoFD = open(fifo.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0777);
            if (fifoFD == -1) {
                perror("Error opening fifo");
                exit(1);
            }
            dup2(fifoFD, STDOUT_FILENO);
            close(fifoFD);
        }

        chainExe(lineCmd);
    }

    else {
        //parent code
         if (!lineCmd.at(0).isBackground){
            waitpid(lcPid, NULL, 0);
        }
        else{
            for(int i=0; i<lineCmd.size(); i++) {
                for(int j=0; j<lineCmd[i].args->size(); j++) {
                    myJobs[totalJobs].cmd.append(" ");
                    myJobs[totalJobs].cmd.append(lineCmd[i].args->at(j));//append each piece of the command
                }
            }
            myJobs[totalJobs].cmd.append(" &");
            myJobs[totalJobs].id = nextId;
            myJobs[totalJobs].pid = lcPid;
            std::cout << "[" << nextId << "] PID: " << lcPid << std::endl;
            totalJobs++;
            nextId++;
            
        }
    }
}

void echo(std::vector<std::string> args){  
    for(int i = 1; i < args.size(); i++){
        if (args[i][0] == '$') {
            string envVar = "";
            envVar = getenv(args[i].erase(0, 1).c_str());
            // if (getenv(args[i].erase(0, 1).c_str()) == NULL) {
            //     std::cout<<"Environment Variable not found"<<" ";
            // }
            std::cout<<envVar<<" ";
        } else {
            std::cout << args[i] << " ";
        }
    }
    std::cout << std::endl;
}

void pwd(){
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    std::cout << cwd << std::endl;
}

void jobs(){
    for(int i=0; i<totalJobs; i++) {
        std::cout << "[" << myJobs[i].id << "] " << myJobs[i].pid << myJobs[i].cmd << std::endl;
    }
}

void kill(std::vector<std::string> args){
    kill(stoi(args.at(2)), stoi(args.at(1)));
}

// void exportVar(std::vector<std::string> args){
//     if(args.size() == 1){
//         std::cout << "Error: No arguments given" << std::endl;
//     }else{
//         std::string var = args[1];
//         std::string val = args[2];
//         setenv(var.c_str(), val.c_str(), 1);
//     }
// }

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