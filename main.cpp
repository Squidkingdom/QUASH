#include <iostream>
#include <stack>
#include <algorithm>
#include <sstream>

int precedence(char op){
    if(op == '<') return 1;
    if(op == '|') return 2;
    if(op == '>') return 3;
    if(op == '&') return 4;
    return 0;
}

std::string infixToPrefix(std::string infix){
    std::stack<char> operators;
    std::stack<std::string> operands;

    std::stringstream ss(infix);
    std::string token;

    while(ss >> token){
        if(token == "<" || token == "|" || token == ">" || token == "&"){
            while(!operators.empty() && precedence(operators.top()) >= precedence(token[0])){
                std::string operand2 = operands.top(); operands.pop();
                std::string operand1 = operands.top(); operands.pop();
                std::string _operator(1, operators.top()); operators.pop();
                operands.push(_operator + " " + operand1 + " " + operand2);
            }
            operators.push(token[0]);
        } else {
            operands.push(token);
        }
    }

    while(!operators.empty()){
        std::string operand2 = operands.top(); operands.pop();
        std::string operand1 = operands.top(); operands.pop();
        std::string _operator(1, operators.top()); operators.pop();
        operands.push(_operator + " " + operand1 + " " + operand2);
    }

    return operands.top();
}

int main(){
    std::string unix_command = "cat < files.txt | grep '*.c' > out.txt &";
    std::cout << infixToPrefix(unix_command) << std::endl;
    return 0;
}