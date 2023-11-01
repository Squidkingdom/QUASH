#include "built-ins.h"

int totalJobs;
int nextId;
struct Process myJobs[5];
string fifoDir = "/tmp/Quash/";


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
    if (args.size() < 2) {
        std::cout << "Error: Not enough arguments" << std::endl;
        return;
    }
    int sig;
    if (args.size() == 2) {
        sig = SIGTERM;

    }else{
        sig = stoi(args.at(2));
    }
    printf("Killing %d with signal %d\n", stoi(args.at(1)), sig);
    kill(stoi(args.at(1)), sig);
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

string tag(){
    usleep(2000);
    return "[QUASH]$ ";
}

vector<string> split (const string &s, char delim) {
    vector<string> result;
    stringstream ss (s);
    string item;

    while (getline (ss, item, delim)) {
        item = trim(item, ' ');
        result.push_back (item);
    }

    return result;
}

string trim(string s, char delim) {
    int i = s.length();
    if (i == 0) {
        return s;
    }
    if (s[0] == delim) {
        s.erase(0, 1);
    }
    if (s[i-1] == delim) {
        s.erase(i-1, 1);
    }
    return s;
}