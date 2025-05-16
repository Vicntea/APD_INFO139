#include "Automaton.hpp"
#include <iostream>

Automaton* createAutomaton(bool acceptByFinalState, Stack* stack) {
    Automaton* a = new Automaton;
    a->acceptByFinalState = acceptByFinalState;
    a->initialState = nullptr;
    a->stack = stack;
    return a;
}

void destroyAutomaton(Automaton* a) {
    for (auto& [_, state] : a->states) {
        for (Transition* t : state->transitions)
            delete t;
        delete state;
    }
    delete a;
}

void addState(Automaton* a, const std::string& name, bool isFinal) {
    State* s = new State{name, isFinal};
    a->states[name] = s;
}

void setInitialState(Automaton* a, const std::string& name) {
    if (a->states.find(name) == a->states.end())
        addState(a, name, false);
    a->initialState = a->states[name];
}

void addTransition(Automaton* a, const std::string& from, char input, char stackTop, const std::string& to, const std::string& replace) {
    if (a->states.find(from) == a->states.end())
        addState(a, from, false);
    if (a->states.find(to) == a->states.end())
        addState(a, to, false);

    Transition* t = new Transition{a->states[from], input, stackTop, a->states[to], replace};
    a->states[from]->transitions.push_back(t);
}

bool simulate(Automaton* a, const std::string& input, std::string& finalStateOut) {
    if (!a->initialState || !a->stack) return false;

    State* currentState = a->initialState;
    resetStack(a->stack);
    pushString(a->stack, "Z");

    size_t i = 0;
    bool progress = true;

    while (progress) {
        progress = false;
        char currentInput = i < input.size() ? input[i] : '~';

        for (Transition* t : currentState->transitions) {
            if ((t->inputSymbol == currentInput || t->inputSymbol == '~') && t->stackTop == topStack(a->stack)) {
                popStack(a->stack);
                pushString(a->stack, t->stackReplace);
                currentState = t->toState;

                if (t->inputSymbol != '~') ++i;

                progress = true;
                break;
            }
        }

        // Si no hay transiciones posibles y aún queda input, rechazamos
        if (!progress && i < input.size()) {
            finalStateOut = currentState->name;
            return false;
        }
    }

    finalStateOut = currentState->name;

    // Aceptación por estado final requiere:
    // - haber consumido toda la entrada
    // - estar en un estado final
    if (a->acceptByFinalState) {
        return (i == input.size() && currentState->isFinal);
    } else {
        return isStackEmpty(a->stack);
    }
}
