#ifndef BUILT_INS_H
#define BUILT_INS_H

#include <vector>
#include <string>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <filesystem>
#include <iostream>
#include "executive.h"
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

struct Process
{
  int id;
  int pid;
  bool isBackground;
  string cmd = "";
};

//extern from executive.h


void cd(std::vector<std::string> args);
void exportVar(std::vector<std::string> args);
void echo(std::vector<std::string> args);
void pwd();
void jobs();
void kill(std::vector<std::string> args);
void clearFifo();
vector<string> split (const string &s, char delim);
string trim(string s, char delim);
std::string tag();

#endif // BUILT_INS_H
