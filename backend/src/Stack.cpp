// .src/Stack.cpp
#include "Stack.hpp"

void Stack::pushString(const std::string& str) {
    for (auto it = str.rbegin(); it != str.rend(); ++it) {
        stack.push(*it);
    }
}

void Stack::pop() {
    if (!stack.empty()) stack.pop();
}

char Stack::top() const {
    return stack.empty() ? '$' : stack.top();
}

bool Stack::empty() const {
    return stack.empty();
}

void Stack::reset() {
    while (!stack.empty()) stack.pop();
}
