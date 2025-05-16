#include "Stack.hpp"

void initStack(Stack* s) {
    while (!s->data.empty()) s->data.pop();
}

void resetStack(Stack* s) {
    initStack(s);
}

void pushString(Stack* s, const std::string& str) {
    for (auto it = str.rbegin(); it != str.rend(); ++it) {
        s->data.push(*it);
    }
}

void popStack(Stack* s) {
    if (!s->data.empty()) s->data.pop();
}

char topStack(const Stack* s) {
    return s->data.empty() ? '$' : s->data.top();
}

bool isStackEmpty(const Stack* s) {
    return s->data.empty();
}
