#include "parser.h"
#include "command.h"


int main() {

    printf("Loaded: $");
    //read from c-in or load default test cmd
    string TSTCMD;
    getline(cin, TSTCMD);
    if (TSTCMD == "") {
        TSTCMD = "cat < files.txt > $HOME/out.txt";
    }
    printf("%s", expandEnvironmentVariables(TSTCMD));
    exit(0);

    // string TSTCMD = "cat < files.txt | grep '*.c' > out.txt &";
    vector<struct Command> linecmd;
    tokenize(TSTCMD, &linecmd);
    setupIPC(&linecmd);
    
}