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
#include "executive.h"
#include "built-ins.h"
#include <alloca.h>

extern int totalJobs;
extern int nextId;
extern struct Process myJobs[5];
extern string fifoDir;

void handleChildDone(int s)
{
    pid_t pid;
    int status;
    // printf("handling child done\n");
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
    {
        // printf("Job with pid %d done\n", pid);
        for (int i = 0; i < totalJobs; i++)
        {
            if (myJobs[i].pid == pid)
            {
                string fifo = fifoDir + "/" + to_string(myJobs[i].id);
                string line;
                ifstream fifoStream(fifo);

                while (getline(fifoStream, line))
                {
                    cout << line << endl;
                }
                fifoStream.close();
                remove(fifo.c_str());
                std::cout << "[" << myJobs[i].id << "] " << myJobs[i].pid << " Done" << std::endl
                          << tag();
                flush(std::cout);

                myJobs[i].pid = 0;
                myJobs[i].id = 0;
            }
        }
    }
}

void spawnExecutor(vector<struct Command> lineCmd)
{
    pid_t execPid;

    // check for built in commands that dont need to be forked
    // This has the side effect of not allowingpipes, redirects of BG to work with built in commands
    if (filterCMD(lineCmd))
    {
        return;
    }

    /* check for background flag
        if we're a background process, set up a file to read/write the output to
        we do this so that we can print the output of the background process when it finishes
        without having to worry about the output interupting normal shell use for slow producing process
        this redirect happens in the parent process of the executor post-fork
        but for now we need to set up the handler for when the executor finishes so we can print the output
        and remove the job from the job list.
    */

    int bg = lineCmd[0].isBackground;
    if (bg)
    {
        signal(SIGCHLD, handleChildDone);
    }

    // TODO RENAME LCPID
    /*
    Fork a new process (execPid)
    this is the 'executor' process whic
    */
    execPid = fork();
    if (execPid == 0)
    {
        signal(SIGCHLD, SIG_DFL); // Reset signal handler to default, we dont want the childen of executor process to inherit the handler
        // redirect output of executor to a file if we're a background process
        if (bg)
        {
            string fifo = (fifoDir + to_string(nextId));
            clearFifo();
            // I actually dont make it a fifo, but a file
            // This is because fifos must have an open reader and writer to get the FD
            // I dont want to have to open a reader yet, so I just make it a file
            int fifoFD = open(fifo.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0777);
            if (fifoFD == -1)
            {
                perror("Error opening fifo");
                exit(1);
            }
            dup2(fifoFD, STDOUT_FILENO);
            close(fifoFD);
        }

        // execute the command
        setupIPC(lineCmd);
    }

    // PARENT OF EXECUTOR
    else
    {
        // wait for the executor to finish if we're not running a background process
        if (!lineCmd.at(0).isBackground)
        {
            waitpid(execPid, NULL, 0);
        }
        // Since we're running a background process we need to add it to the job list so that we can print the output when it finishes
        else
        {
            for (int i = 0; i < lineCmd.size(); i++)
            {
                for (int j = 0; j < lineCmd[i].args.size(); j++)
                {
                    myJobs[totalJobs].cmd.append(" ");
                    myJobs[totalJobs].cmd.append(lineCmd[i].args.at(j)); // append each piece of the command
                }
            }
            myJobs[totalJobs].cmd.append(" &"); // append the & to the end of the command because the parser removes it after it sets the flag
            myJobs[totalJobs].id = nextId;
            myJobs[totalJobs].pid = execPid;
            std::cout << "[" << nextId << "] PID: " << execPid << std::endl;
            totalJobs++;
            nextId++;
        }
    }
}

// Responsible for executing chained or complex commands
void setupIPC(vector<struct Command> lineCmd)
{

    size_t nCmds = lineCmd.size();
    vector<pid_t> pid(nCmds);
    vector<int[2]> fd(nCmds - 1);

    // Given N commands, we need N-1 pipes to connect them
    for (int i = 0; i < nCmds - 1; i++)
    {
        pipe(fd[i]);
    }

    // Loop through and fork each process
    // Because each fork has access to the already made pipes we can assign pipes ad-hoc
    for (int i = 0; i < nCmds; i++)
    {
        pid[i] = fork();

        // CHILD CODE
        if (pid[i] == 0)
        {
            struct Command cmd = lineCmd.at(i); // get whatever command we're working on.

            // If we have to read from a pipe, set the input to the pipe
            // fd[i-1] because we have to be at least subcmd index 1, and the pipe we need to read from is index 0
            if (HAS_PIPE)
            {
                dup2(fd[i - 1][0], STDIN_FILENO);
            }

            // If we have to write to a pipe, set the output to the pipe
            // fd[i] because the pipe we are writing out to will always have the same index as the curr subcmd.
            if (WILL_PIPE)
            {
                dup2(fd[i][1], STDOUT_FILENO);
            }

            // If we have to read from a file, set the input to the file
            if (cmd.hasRead)
            {
                int fdin = 0;
                fdin = open(cmd.readFrom.c_str(), O_RDONLY);
                // error check file open
                if (fdin == -1)
                {
                    std::cerr << "Error: Cannot open file: " << cmd.readFrom << std::endl;
                    exit(1);
                }
                dup2(fdin, STDIN_FILENO);
                close(fdin);
            }

            // If we have to write to a file, set the output to the file. use O_TRUNC to overwrite the file
            if (cmd.hasRedirect)
            {
                int fdout = 0;
                fdout = open(cmd.redirectTo.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0777);
                dup2(fdout, STDOUT_FILENO);
                // execute
                close(fdout);
            }

            // If we have to append to a file, set the output to the file, use O_APPEND to append to the file
            if (cmd.redirectAppend)
            {
                int fdout = 0;
                fdout = open(cmd.redirectTo.c_str(), O_CREAT | O_WRONLY | O_APPEND, 0777);
                dup2(fdout, STDOUT_FILENO);
                // execute
                close(fdout);
            }

            // call execute and then exit
            execute(cmd);
            exit(0);
        }

        else
        {
            // parent code
            struct Command cmd = lineCmd.at(i);
            if (HAS_PIPE)
            {
                close(fd[i - 1][0]);
            }
            if (WILL_PIPE)
            {
                close(fd[i][1]);
                // read from the pipe to see if wer sent anything
            }
            // printf("Waiting...\n");
            waitpid(pid[i], NULL, 0);
        }
    }
    wait(NULL);
    close(1);
    exit(0);
}

// Checks if we're running a chainable built-ins and calls exec if not.
// Some cmds are not chainable because they must be run by the shell process (before we fork the executor)
void execute(struct Command cmd)
{
    std::string bin = cmd.args.at(0);
    if (bin == "echo")
    {
        echo(cmd.args);
    }
    else if (bin == "pwd")
    {
        pwd();
    }
    else if (bin == "exit" || bin == "quit")
    {
        exit(0);
    }
    else if (bin == "jobs")
    {
        jobs();
    }
    else if (bin == "kill")
    {
        kill(cmd.args);
    }
    else if (bin == "export")
    {
        exportVar(cmd.args);
    }
    // Not a built in, so we have to exec
    else
    {   
        //Bit of GPT4 generated code to convert vector<string> to char**
        //Got stuck trying to avoid using new, or malloc, or any other dynamic memory allocation
        char **args = (char **)alloca((cmd.args.size() + 1) * sizeof(char *));
        for (int i = 0; i < cmd.args.size(); i++)
        {
            args[i] = const_cast<char *>(cmd.args.at(i).c_str());
        }
        args[cmd.args.size()] = NULL;
        execvp(bin.c_str(), args);
        std::cerr << "Error: Command not found: " << bin << std::endl;
        exit(1);
    }
    // we have to exit if we call a built-in
    exit(0);
}

bool filterCMD(vector<Command> cmd)
{
    if (cmd[0].args.at(0) == "export")
    {
        exportVar(cmd[0].args);
        return true;
    }
    if (cmd[0].args.at(0) == "exit" || cmd[0].args.at(0) == "quit")
    {
        exit(0);
        return true;
    }
    if (cmd[0].args.at(0) == "cd")
    {
        cd(cmd[0].args);
        return true;
    }
    return false;
}
