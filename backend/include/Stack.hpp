// ./include/Stack.hpp

#pragma once
#include <stack>
#include <string>

class Stack {
public:
    void pushString(const std::string& str);
    void pop();
    char top() const;
    bool empty() const;
    void reset();
private:
    std::stack<char> stack;
};
