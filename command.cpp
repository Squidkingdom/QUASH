#include <iostream>

std::string echo(std::string arguments[]) {
    std::cout<<arguments[1];
    return arguments[1];
};



int main(std::string arguments[]) {
    if (arguments[0] == "echo") {
        echo(arguments);
    }
    return 0;
}