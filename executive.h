#ifndef EXECUTIVE_H
#define EXECUTIVE_H
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
#include "built-ins.h"
#include <unistd.h>

using namespace std;


// If there are more than one subcommands, and this is not the first subcommand, and the previous we're not reading in from a file, then we must be reading in from a pipe
#define HAS_PIPE (nCmds > 1) && (i != 0) && !cmd.hasRead
// If there are more than one subcommands, and this is not the last subcommand, and we're not writing to a file, then we must be writing to a pipe
#define WILL_PIPE (nCmds > 1) && (i != nCmds-1) && !cmd.hasRedirect && !cmd.redirectAppend

void execute(struct Command cmd);
string tag();
void setupIPC(vector<struct Command> lineCmd);
bool filterCMD(vector<Command> cmd);
void spawnExecutor(vector<struct Command> lineCmd);
#endif

