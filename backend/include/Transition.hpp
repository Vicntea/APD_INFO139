// ./include/Transition.hpp
#pragma once
#include <string>

struct Transition {
    std::string fromState;
    char inputSymbol;
    char stackTop;
    std::string toState;
    std::string stackReplace;
};
