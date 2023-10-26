#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <strings.h>
#include <errno.h>
#include <sys/wait.h>

#define BASH_EXEC "/bin/bash"
#define FIND_EXEC "/bin/find"
#define XARGS_EXEC "/usr/bin/xargs"
#define GREP_EXEC "/bin/grep"
#define SORT_EXEC "/bin/sort"
#define HEAD_EXEC "/usr/bin/head"

// Brady Holland
// 3028989
// EECS 678 Operating Systems Lab 3
// 9/17/2023 11:54 PM
int main(int argc, char *argv[])
{
    int status;
    pid_t pid_1, pid_2, pid_3, pid_4;
    int p1[2], p2[2], p3[2], p4[2];
    pipe(p1); pipe(p2); pipe(p3);

    pid_1 = fork();
    if (pid_1 == 0)
    {   //Code from the slides on how to exec
        char cmdbuf[100];
        bzero(cmdbuf, 100);
        sprintf(cmdbuf, "%s %s -name \'*\'.[ch]", FIND_EXEC, argv[1]);

        // Redirect child proccess 1's stdout to pipe p1's write end
        dup2(p1[1], STDOUT_FILENO);

        //Exec thing.
        execl(BASH_EXEC, BASH_EXEC, "-c", cmdbuf, (char *)0);

        //Does this exit actually do anything?
        exit(0);
    }

    close(p1[1]); // close pipe p1's write end

    pid_2 = fork();
    if (pid_2 == 0)
    {
        //setup for exec later
        char cmdbuf[100];
        bzero(cmdbuf, 100);
        sprintf(cmdbuf, "%s %s -c %s", XARGS_EXEC, GREP_EXEC, argv[2]);

        // redirect stdin to pipe p1's read end
        dup2(p1[0], STDIN_FILENO); 
        //redirect stdout to pipe p2's write end
        dup2(p2[1], STDOUT_FILENO);
        execl(BASH_EXEC, BASH_EXEC, "-c", cmdbuf, (char *)0);
        exit(0);
    }
    close(p1[0]); // close pipe p1's read end; p1 is completely closed
    close(p2[1]); // close pipe p2's write end

    pid_3 = fork();
    if (pid_3 == 0)
    {
        //setup for exec later
        char cmdbuf[100];
        bzero(cmdbuf, 100);
        sprintf(cmdbuf, "%s -t: +1.0 -2.0 --numeric --reverse", SORT_EXEC);

        // redirect stdin to pipe p2's read end
        dup2(p2[0], STDIN_FILENO); 
        //redirect stdout to pipe p3's write end
        dup2(p3[1], STDOUT_FILENO);

        execl(BASH_EXEC, BASH_EXEC, "-c", cmdbuf, (char *)0);
        exit(0);
    }
    close(p2[0]); // close pipe p2's read end; p2 is completely closed
    close(p3[1]); // close pipe p3's write end

    pid_4 = fork();
    if (pid_4 == 0)
    {
        //setup for exec later
        char cmdbuf[100];
        bzero(cmdbuf, 100);
        sprintf(cmdbuf, "%s --lines=%s", HEAD_EXEC, argv[3]);

        // redirect stdin to pipe p3's read end
        dup2(p3[0], STDIN_FILENO); // redirect stdin to pipe p1's read end
        
        //exec the thing
        execl(BASH_EXEC, BASH_EXEC, "-c", cmdbuf, (char *)0);
        exit(0);
    }
    close(p3[0]); // close pipe p3's read end; p3 is completely closed

    if ((waitpid(pid_1, &status, 0)) == -1)
    {
        fprintf(stderr, "Process 1 encountered an error. ERROR%d", errno);
        return EXIT_FAILURE;
    }
    if ((waitpid(pid_2, &status, 0)) == -1)
    {
        fprintf(stderr, "Process 2 encountered an error. ERROR%d", errno);
        return EXIT_FAILURE;
    }
    if ((waitpid(pid_3, &status, 0)) == -1)
    {
        fprintf(stderr, "Process 3 encountered an error. ERROR%d", errno);
        return EXIT_FAILURE;
    }
    if ((waitpid(pid_4, &status, 0)) == -1)
    {
        fprintf(stderr, "Process 4 encountered an error. ERROR%d", errno);
        return EXIT_FAILURE;
    }
}