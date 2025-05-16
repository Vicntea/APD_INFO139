#include "Automaton.hpp"
#include "Stack.hpp"
#include <iostream>

int main() {
    Stack pila;
    initStack(&pila);

    // Automaton que acepta por estado final
    Automaton* a = createAutomaton(true, &pila);

    // Define solo transiciones. Los estados se crean automáticamente si no existen
    setInitialState(a, "q0");

    // Transiciones para aⁿ (apila 'A' por cada 'a')
    addTransition(a, "q0", 'a', 'Z', "q0", "AZ");
    addTransition(a, "q0", 'a', 'A', "q0", "AA");

    // Transición que cambia de fase: primer 'b'
    addTransition(a, "q0", 'b', 'A', "q1", "");

    // Transiciones para bⁿ (desapila una 'A' por cada 'b')
    addTransition(a, "q1", 'b', 'A', "q1", "");

    // Cuando la pila vuelve a tener 'Z', y ya no quedan más símbolos que leer:
    // transición lambda para ir a estado final
    addTransition(a, "q1", '~', 'Z', "q2", "");

    // Declaramos q2 como estado final
    a->states["q2"]->isFinal = true;

    // Leer palabra
    std::string palabra;
    std::cout << "Ingrese una palabra (a^n b^n): ";
    std::cin >> palabra;

    std::string estadoFinal;
    bool aceptada = simulate(a, palabra, estadoFinal);

    std::cout << "Estado final: " << estadoFinal << "\n";
    std::cout << (aceptada ? "Palabra aceptada" : "Palabra rechazada") << std::endl;

    destroyAutomaton(a);
    return 0;
}
