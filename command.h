#ifndef COMMAND_H
#define COMMAND_H
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
typedef struct job {
        int id;
        char *name;
        pid_t pid;
        int status;
        char *descriptor;
        vector<string> cmd;
        struct job *next;
} t_job;

#define HAS_PIPE (nCmds > 1) && (i != 0) && !cmd.hasRead
#define WILL_PIPE (nCmds > 1) && (i != nCmds-1) && !cmd.hasRedirect && !cmd.redirectAppend

void execute(struct Command cmd);
void setupIPC(vector<struct Command> lineCmd);
#endif

