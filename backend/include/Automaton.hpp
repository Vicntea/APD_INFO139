#pragma once
#include "Transition.hpp"
#include "Stack.hpp"

#include <unordered_map>
#include <string>
#include <vector>

struct State {
    std::string name;
    bool isFinal;
    std::vector<Transition*> transitions;
};

struct Automaton {
    bool acceptByFinalState;
    State* initialState;
    Stack* stack;
    std::unordered_map<std::string, State*> states;
};

Automaton* createAutomaton(bool acceptByFinalState, Stack* stack);
void destroyAutomaton(Automaton* automaton);

void addState(Automaton* automaton, const std::string& name, bool isFinal);
void setInitialState(Automaton* automaton, const std::string& name);
void addTransition(Automaton* automaton, const std::string& from, char input, char stackTop, const std::string& to, const std::string& replace);
bool simulate(Automaton* automaton, const std::string& input, std::string& finalStateOut);
