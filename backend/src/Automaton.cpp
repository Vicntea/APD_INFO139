#include "Automaton.hpp"
#include <iostream>

Automaton::Automaton(bool acceptByFinal) : acceptByFinalState(acceptByFinal) {}

void Automaton::setInitialState(const std::string& state) {
    initialState = state;
    // Si el estado no existe, se agrega como no final
    if (states.find(state) == states.end())
        states[state] = {state, false};
}

void Automaton::addState(const std::string& stateName, bool isFinal) {
    states[stateName] = {stateName, isFinal};
}

void Automaton::addTransition(const Transition& t) {
    transitions[t.fromState].push_back(t);
}

bool Automaton::simulate(const std::string& input, std::string& finalStateOut) {
    std::string currentState = initialState;
    stack.reset();
    stack.pushString("Z");

    size_t i = 0;
    while (i <= input.size()) {
        char currentInput = i < input.size() ? input[i] : '~';
        bool transitioned = false;

        for (const auto& t : transitions[currentState]) {
            if ((t.inputSymbol == currentInput || t.inputSymbol == '~') && t.stackTop == stack.top()) {
                stack.pop();
                stack.pushString(t.stackReplace);
                currentState = t.toState;
                if (t.inputSymbol != '~') i++;
                transitioned = true;
                break;
            }
        }

        if (!transitioned) break;
    }

    finalStateOut = currentState;

    if (acceptByFinalState) {
        return states[currentState].isFinal;
    } else {
        return stack.empty();
    }
}
