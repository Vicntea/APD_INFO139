#pragma once
#include "Transition.hpp"
#include "Stack.hpp"
#include <unordered_map>
#include <vector>
#include <string>

// Estructura para representar un estado
struct State {
    std::string name;
    bool isFinal = false;
};

class Automaton {
public:
    Automaton(bool acceptByFinal);
    void setInitialState(const std::string& state);
    void addState(const std::string& stateName, bool isFinal);
    void addTransition(const Transition& t);
    bool simulate(const std::string& input, std::string& finalStateOut);

private:
    std::string initialState;
    bool acceptByFinalState;
    std::unordered_map<std::string, State> states;
    std::unordered_map<std::string, std::vector<Transition>> transitions;
    Stack stack;
};
