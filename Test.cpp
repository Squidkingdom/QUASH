#include "parser.h"
#include "command.h"
void setupIPC(vector<struct Command>* lineCmd);

int main() {
    string TSTCMD = "cat -n < animals | grep '*.c' > out.txt";

    // string TSTCMD = "cat < files.txt | grep '*.c' > out.txt &";
    vector<struct Command> linecmd;
    tokenize(TSTCMD, &linecmd);
    setupIPC(&linecmd);
    
}