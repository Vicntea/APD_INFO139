#include "Automaton.hpp"
#include <iostream>

void modo_consola() {
    Automaton apd(true); // true = acepta por estado final

    // Definir estados
    apd.addState("q0", false);
    apd.addState("q1", false);
    apd.addState("qf", true); // estado final

    apd.setInitialState("q0");

    // Transiciones
    apd.addTransition({"q0", 'a', 'Z', "q0", "AZ"});
    apd.addTransition({"q0", 'a', 'A', "q0", "AA"});
    apd.addTransition({"q0", 'b', 'A', "q1", ""});
    apd.addTransition({"q1", 'b', 'A', "q1", ""});
    apd.addTransition({"q1", ' ', 'Z', "qf", "Z"}); // transición epsilon

    std::string input;
    std::cout << "Ingrese palabra: ";
    std::cin >> input;

    std::string estadoFinal;
    if (apd.simulate(input, estadoFinal)) {
        std::cout << "Palabra aceptada. Estado final alcanzado: " << estadoFinal << std::endl;
    } else {
        std::cout << "Palabra no aceptada. Estado final alcanzado: " << estadoFinal << std::endl;
    }
}

int main() {
    int opcion = 0;
    std::cout << "=== Simulador de Autómata Pushdown ===" << std::endl;
    std::cout << "1. Modo Consola" << std::endl;
    std::cout << "Seleccione una opción: ";
    std::cin >> opcion;

    if (opcion == 1) {
        modo_consola();
    } else {
        std::cout << "Opción no válida." << std::endl;
    }

    return 0;
}
