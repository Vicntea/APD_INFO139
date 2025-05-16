#pragma once
#include <stack>
#include <string>

struct Stack {
    std::stack<char> data;
};

void initStack(Stack* s);
void resetStack(Stack* s);
void pushString(Stack* s, const std::string& str);
void popStack(Stack* s);
char topStack(const Stack* s);
bool isStackEmpty(const Stack* s);
