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

    // Map '-' to '~' for the input symbol in transitions
    if (input == '-') {
        input = '~';
    }

    Transition* t = new Transition{a->states[from], input, stackTop, a->states[to], replace};
    a->states[from]->transitions.push_back(t);
}


char simulate(Automaton* a, const std::string& input, std::string& finalStateOut) {
    if (!a->initialState || !a->stack) {
        return 'r'; // Indica rechazo debido a una configuración de autómata inválida
    }

    State* currentState = a->initialState;
    resetStack(a->stack);
    pushString(a->stack, "R"); // Empuja el símbolo inicial de la pila 'Z'

    size_t i = 0;
    bool progress = true;

    while (progress) {
        progress = false;
        char currentInput = i < input.size() ? input[i] : '~'; // '~' representa epsilon en la entrada

        // Crear una lista temporal para almacenar transiciones aplicables
        std::vector<Transition*> applicableTransitions;
        for (Transition* t : currentState->transitions) {
            if ((t->inputSymbol == currentInput || t->inputSymbol == '~') && t->stackTop == topStack(a->stack)) {
                applicableTransitions.push_back(t);
            }
        }

        // Priorizar transiciones no-epsilon sobre transiciones epsilon si ambas son aplicables
        Transition* chosenTransition = nullptr;
        for (Transition* t : applicableTransitions) {
            if (t->inputSymbol != '~') {
                chosenTransition = t;
                break;
            }
        }
        if (!chosenTransition && !applicableTransitions.empty()) {
            chosenTransition = applicableTransitions[0]; // Si solo existen transiciones epsilon, elige la primera
        }

        if (chosenTransition) {
            popStack(a->stack); // Saca el símbolo de la cima de la pila
            pushString(a->stack, chosenTransition->stackReplace); // Empuja la cadena de reemplazo (maneja epsilon)
            currentState = chosenTransition->toState; // Cambia al siguiente estado

            if (chosenTransition->inputSymbol != '~') ++i; // Solo avanza en la entrada si no es una transición epsilon

            progress = true; // Se realizó una transición, continuamos el bucle
        }

        // Si no hay transiciones posibles Y aún queda input, rechazamos (no se pudo avanzar)
        if (!progress && i < input.size()) {
            finalStateOut = currentState->name;
            return 'r';
        }
    }

    finalStateOut = currentState->name;

    // La palabra es aceptada si:
    // 1. Se ha consumido toda la entrada (i == input.size()) AND
    // 2. (El autómata está en un estado final OR la pila está vacía)

    // Primero, aseguramos que toda la entrada ha sido consumida.
    if (i != input.size()) {
        return 'r'; // Si no se consumió toda la entrada, no es aceptada.
    }

    // Luego, verificamos si cumple el criterio de aceptación (estado final O pila vacía)
    if (currentState->isFinal && isStackEmpty(a->stack)) {
        // Si ambas condiciones se cumplen, prioriza el estado final como convención común.
        return 'e';
    } else if (currentState->isFinal) {
        return 'e';
    } else if (isStackEmpty(a->stack)) {
        return 's';
    } else {
        return 'r';
    }
}
