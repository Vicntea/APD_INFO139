#pragma once
#include <string>

struct State; // Declaración adelantada

struct Transition {
    State* fromState;
    char inputSymbol;
    char stackTop;
    State* toState;
    std::string stackReplace;
};
