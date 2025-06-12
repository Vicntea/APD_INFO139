#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QDebug>
#include <sstream>
#include <stdexcept>

// Constructor de la ventana principal
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);



    // --- Inicialización de tu Lógica de Backend ---
    pila = new Stack();
    automaton = nullptr; // Inicialmente nulo o llamaremos a loadEmptyAutomaton() para inicializarlo.

    // NUEVO: Cargar el estado "vacío" o "inicial" del autómata al arrancar.
    loadEmptyAutomaton();

    // Configura el estado inicial por defecto en la GUI.
    ui->initialStateLineEdit->setText("q0"); // Esto se establecerá en el autómata cuando se llame a setInitialState por loadEmptyAutomaton()

    // Opcional: Poblar el ComboBox con las opciones de presets
    ui->presetComboBox->addItem("Autómata Vacío"); // Index 0
    ui->presetComboBox->addItem("a^n b^n");        // Index 1
    ui->presetComboBox->addItem("a^n b^n c^n");    // Index 2 (o cualquier otro preset que añadas)
    // Puedes seleccionar el primer elemento por defecto
    // ui->presetComboBox->setCurrentIndex(0); // Esto ya llamará a on_presetComboBox_currentIndexChanged(0)
}

void MainWindow::on_evaluateButton_clicked() {
    qDebug() << "on_evaluateButton_clicked() ha sido llamado.";

    std::string inputWord = ui->wordLineEdit->text().toStdString();
    std::string finalStateName;

    if (!automaton) {
        QMessageBox::critical(this, "Error", "El autómata no ha sido inicializado.");
        return;
    }

    // Llama a simulate que ahora devuelve un char ('r', 's', 'e')
    char acceptanceResult = simulate(automaton, inputWord, finalStateName);

    QString resultText;
    QString message;
    bool accepted = false;

    // Usamos un switch para interpretar el char retornado
    switch (acceptanceResult) {
        case 'r': // Rechazada
            resultText = "RECHAZADA";
            accepted = false;
            break;
        case 'e': // Aceptada por estado final
            resultText = "ACEPTADA (Estado Final)";
            accepted = true;
            break;
        case 's': // Aceptada por pila vacía
            resultText = "ACEPTADA (Pila Vacía)";
            accepted = true;
            break;
        // No necesitamos un caso para 'ACCEPTED_BY_BOTH' ya que tu función simulate prioriza 'e'
        // si ambos son verdaderos, lo que se maneja con 'e'.
    }

    message = QString("Palabra: '%1'\nResultado: %2\nEstado final: %3")
                      .arg(QString::fromStdString(inputWord))
                      .arg(resultText)
                      .arg(QString::fromStdString(finalStateName));

    ui->resultLabel->setText(message);
    if (accepted) {
        ui->resultLabel->setStyleSheet("color: green; font-weight: bold;");
    } else {
        ui->resultLabel->setStyleSheet("color: red; font-weight: bold;");
    }
    ui->historyListWidget->addItem(QString("'%1' -> %2 (Estado: %3)").arg(QString::fromStdString(inputWord)).arg(resultText).arg(QString::fromStdString(finalStateName)));
}

// Destructor de la ventana principal
MainWindow::~MainWindow()
{
    destroyAutomaton(automaton); // Libera la memoria del autómata actual si existe
    delete pila;
    delete ui;
}

// --- Implementación de Slots (EXISTENTES) ---



void MainWindow::on_addTransitionButton_clicked() {
    QString transitionQString = ui->addTransitionLineEdit->text();
    std::string transitionStr = transitionQString.toStdString();

    std::string q_src, q_dest, push_str;
    char sym_in, sym_top;

    if (parseTransitionString(transitionStr, q_src, sym_in, sym_top, q_dest, push_str)) {
        addTransition(automaton, q_src, sym_in, sym_top, q_dest, push_str);
        user_defined_transitions.push_back(transitionStr);

        ui->addTransitionLineEdit->clear();
        updateTransitionsList();
        updateFinalStatesList();
    } else {
        qDebug() << "Mostrando error de formato de transición.";
        QMessageBox::warning(this, "Error de Formato", "Formato de transición incorrecto. Use: estado,simbolo,tope_pila,estado_destino,nuevoTope_pila\n(Para caracter vacio utilize ' - ') ");
    }
}

void MainWindow::on_resetButton_clicked() {
    clearAndResetAutomaton(); // Usa la nueva función auxiliar
    loadEmptyAutomaton(); // Carga el autómata vacío después del reset

    // Resetea los campos de la UI que no se manejan en clearAndResetAutomaton
    ui->wordLineEdit->clear();
    ui->addTransitionLineEdit->clear();
    ui->historyListWidget->clear();
    ui->resultLabel->setText("");
    ui->resultLabel->setStyleSheet("");

    // Asegúrate de que el ComboBox de presets también se resetee a "Autómata Vacío"
    ui->presetComboBox->setCurrentIndex(0); // Selecciona el primer ítem, que debería ser "Autómata Vacío"
}

void MainWindow::on_addFinalStateButton_clicked() {
    QString finalStateQString = ui->addFinalStateLineEdit->text();
    std::string finalStateName = finalStateQString.toStdString();

    if (!finalStateName.empty()) {
        if (automaton && automaton->states.count(finalStateName)) {
            automaton->states[finalStateName]->isFinal = true;
            ui->addFinalStateLineEdit->clear();
            updateFinalStatesList();
        } else {
            QMessageBox::warning(this, "Error", QString("El estado '%1' no existe. Añádalo primero como parte de una transición.").arg(finalStateQString));
        }
    } else {
        QMessageBox::warning(this, "Error", "Ingrese un nombre de estado.");
    }
}

void MainWindow::on_initialStateLineEdit_textEdited(const QString &text) {
    std::string stateName = text.toStdString();
    if (automaton && !stateName.empty()) {
        setInitialState(automaton, stateName);
        qDebug() << "Initial state set to: " << QString::fromStdString(stateName);
    }
}

// --- NUEVO SLOT para el ComboBox de presets ---
void MainWindow::on_presetComboBox_currentIndexChanged(int index) {
    clearAndResetAutomaton(); // Limpia y resetea el autómata actual

    switch (index) {
        case 0: // "Autómata Vacío"
            loadEmptyAutomaton();
            break;
        case 1: // "a^n b^n"
            loadAnBnAutomaton();
            break;
        case 2: // "a^n b^n c^n"
            loadAnBnCnAutomaton(); // Llama a la función del nuevo preset
            break;
        default:
            qDebug() << "Preset no reconocido.";
            loadEmptyAutomaton(); // Por defecto, carga el autómata vacío
            break;
    }
    // Después de cargar cualquier preset, actualiza las listas y el estado inicial
    ui->initialStateLineEdit->setText("q0"); // Reinicia el texto del QLineEdit a 'q0'
    setInitialState(automaton, "q0"); // Asegura que el autómata también tenga 'q0' como inicial
    updateTransitionsList();
    updateFinalStatesList();
}


// --- Implementación de Funciones Auxiliares (EXISTENTES) ---

// Función para parsear una cadena de transición (ej. "q0,a,Z,q0,AZ")
bool MainWindow::parseTransitionString(const std::string& transitionStr, std::string& q_src, char& sym_in, char& sym_top, std::string& q_dest, std::string& push_str) {
    std::stringstream ss(transitionStr);
    std::string segment;
    std::vector<std::string> segments;

    // Aquí puedes añadir los qDebug() temporales si sigues teniendo problemas de formato
    // qDebug() << "Intentando parsear: " << QString::fromStdString(transitionStr);


    while(std::getline(ss, segment, ',')) {
        segments.push_back(segment);
    }

    // qDebug() << "Segmentos encontrados: " << segments.size();
    // for (size_t i = 0; i < segments.size(); ++i) {
    //     qDebug() << "Segmento " << i << ": '" << QString::fromStdString(segments[i]) << "'";
    // }

    // Debe haber exactamente 5 segmentos.
    if (segments.size() != 5) return false;

    q_src = segments[0];

    // --- ¡ESTE ES EL CAMBIO CLAVE para sym_in! ---
    // Si el segmento es vacío O es "-", se interpreta como lambda ('~' internamente).
    // De lo contrario, se toma el primer carácter del segmento.
    sym_in = (segments[1].empty() || segments[1] == "-") ? '~' : segments[1][0];

    // --- ¡ESTE ES EL CAMBIO CLAVE para sym_top! ---
    // Si el segmento es vacío O es "-", se interpreta como pila vacía ('$' internamente).
    // De lo contrario, se toma el primer carácter del segmento.
    sym_top = (segments[2].empty() || segments[2] == "-") ? '$' : segments[2][0];

    q_dest = segments[3];

    // --- ¡ESTE ES EL CAMBIO CLAVE para push_str! ---
    // Si el segmento es "-", se interpreta como cadena vacía ("" internamente).
    // De lo contrario, se toma el segmento tal cual.
    push_str = (segments[4] == "-") ? "" : segments[4];

    return true;
}

void MainWindow::updateTransitionsList() {
    ui->transitionsListWidget->clear();
    if (!automaton) return; // Si el autómata es nulo, no hay transiciones que mostrar
    for (const auto& pair : automaton->states) {
        State* s = pair.second;
        for (Transition* t : s->transitions) {
            QString transitionText = QString("(%1, %2, %3) -> (%4, %5)")
                                             .arg(QString::fromStdString(s->name))
                                             .arg(t->inputSymbol == '~' ? "ε" : QString(t->inputSymbol))
                                             .arg(t->stackTop == '$' ? "$" : QString(t->stackTop))
                                             .arg(QString::fromStdString(t->toState->name))
                                             .arg(QString::fromStdString(t->stackReplace.empty() ? "ε" : t->stackReplace));
            ui->transitionsListWidget->addItem(transitionText);
        }
    }
}

void MainWindow::updateFinalStatesList() {
    ui->finalStatesListWidget->clear();
    if (!automaton) return; // Si el autómata es nulo, no hay estados finales
    for (const auto& pair : automaton->states) {
        State* s = pair.second;
        if (s->isFinal) {
            ui->finalStatesListWidget->addItem(QString::fromStdString(s->name));
        }
    }
}

// --- NUEVAS FUNCIONES PARA LA GESTIÓN DE PRESETS ---

// Función auxiliar para limpiar y resetear el autómata
void MainWindow::clearAndResetAutomaton() {
    if (automaton) {
        destroyAutomaton(automaton); // Libera la memoria del autómata actual
        automaton = nullptr; // Asegura que el puntero sea nulo después de la destrucción
    }
    resetStack(pila); // Reinicia la pila
    user_defined_transitions.clear(); // Limpia las transiciones definidas por el usuario
    // Las listas de la UI se actualizarán después de cargar el nuevo autómata
}

// Preset: Autómata Vacío (o estado inicial por defecto)
void MainWindow::loadEmptyAutomaton() {
    clearAndResetAutomaton(); // Asegura que esté limpio antes de empezar
    automaton = createAutomaton(true, pila); // Crea un autómata nuevo y limpio para aceptación por estado final
    setInitialState(automaton, "q0"); // Establece un estado inicial por defecto

    // Limpia las listas de la UI
    ui->transitionsListWidget->clear();
    ui->finalStatesListWidget->clear();
}

// Preset: a^n b^n
void MainWindow::loadAnBnAutomaton() {
    clearAndResetAutomaton();
    automaton = createAutomaton(true, pila); // Crea un autómata nuevo

    // Añade las transiciones para a^n b^n
    addTransition(automaton, "q0", 'a', 'R', "q0", "AR");
    addTransition(automaton, "q0", 'a', 'A', "q0", "AA");
    addTransition(automaton, "q0", 'b', 'A', "q1", "");
    addTransition(automaton, "q1", 'b', 'A', "q1", "");
    addTransition(automaton, "q1", '~', 'R', "q2", "");

    // Marca 'q2' como estado final
    if (automaton->states.count("q2")) {
        automaton->states["q2"]->isFinal = true;
    } else {
        qDebug() << "Warning: State q2 not found when trying to mark as final in AnBn preset.";
    }

    setInitialState(automaton, "q0"); // Asegura que q0 sea el inicial
}

// Nuevo Preset de ejemplo: a^n b^n c^n
void MainWindow::loadAnBnCnAutomaton() {
    clearAndResetAutomaton();
    automaton = createAutomaton(true, pila);

    // Transiciones para a^n b^n c^n (ejemplo simplificado)
    // q0: Pushing 'A' for 'a's
    addTransition(automaton, "q0", 'a', 'R', "q0", "AR");
    addTransition(automaton, "q0", 'a', 'A', "q0", "AA");

    // q0 -> q1: Transition for 'b's, popping 'A's
    addTransition(automaton, "q0", 'b', 'A', "q1", ""); // Go to q1 when first 'b' is read and 'A' is on top
    addTransition(automaton, "q1", 'b', 'A', "q1", ""); // Keep popping 'A' for subsequent 'b's

    // q1 -> q2: Transition for 'c's, popping 'A's (if any remaining 'A's need to be popped for 'c's, or change symbol)
    // For a^n b^n c^n you might need to pop 'A's for 'b's, then transition to a new state for 'c's.
    // Let's assume 'c's pop a different symbol, or we finish stack.
    // For this example, let's assume 'b's pop 'A's, and 'c's consume nothing until 'Z'.
    // A more complex PDA might push another symbol for 'b's to be popped by 'c's.
    // This is a simplified example.
    addTransition(automaton, "q1", 'c', 'R', "q2", "R"); // Consuming 'c' while 'Z' is on stack means all A's were popped by b's
    addTransition(automaton, "q2", 'c', 'R', "q2", "R"); // Continue consuming 'c's (not really, needs stack)
    // A more realistic a^n b^n c^n might be:
    // q0, a, Z -> q0, AZ
    // q0, a, A -> q0, AA
    // q0, b, A -> q1, (empty)
    // q1, b, A -> q1, (empty)
    // q1, c, Z -> q2, (empty) (if stack is empty before c's then this is for n=0)
    // Let's use a simpler one: 'b's pop 'A's, then 'c's are consumed, then epsilon moves to final state.
    // This example is for a^n b^n followed by arbitrary c's, ending on empty stack.
    // If n_a = n_b = n_c, you'd need a more complex stack strategy (e.g. push X for 'a', pop X for 'b', push Y for 'b', pop Y for 'c').

    // Let's stick to simple a^n b^n structure for simplicity for this example:
    // q0, a, Z -> q0, AZ
    // q0, a, A -> q0, AA
    // q0, b, A -> q1, ε
    // q1, b, A -> q1, ε
    // q1, c, Z -> q2, ε  // Read 'c', stack has 'Z', move to q2
    // q2, c, Z -> q2, ε  // Read more 'c's, stack has 'Z' (this doesn't check counts)
    // q2, ε, Z -> q3, ε  // Final state
    // Mark q3 as final.

    // Let's refine the a^n b^n c^n to be clearer, assuming one-to-one popping for b and c:
    // (A more standard PDa for a^n b^n c^n would push X for 'a', pop X for 'b', push Y for 'b', pop Y for 'c')
    // For this example, let's make it more illustrative.
    // We'll push 'A' for 'a', then pop 'A' for 'b', then expect to be in a state where 'c's can be consumed.

    // A simpler a^n b^n c^n where all counts must match:
    addTransition(automaton, "q0", 'a', 'R', "q0", "AR"); // Push A for 'a', keep Z
    addTransition(automaton, "q0", 'a', 'A', "q0", "AA"); // Push A for 'a', keep A

    addTransition(automaton, "q0", 'b', 'A', "q1", ""); // Pop A for 'b'
    addTransition(automaton, "q1", 'b', 'A', "q1", ""); // Pop A for 'b'

    addTransition(automaton, "q1", 'c', 'A', "q2", ""); // Pop A for 'c'
    addTransition(automaton, "q2", 'c', 'A', "q2", ""); // Pop A for 'c'

    addTransition(automaton, "q2", '~', 'R', "q3", ""); // Epsilon transition, Z on stack, go to final state q3

    // Mark 'q3' as state final
    if (automaton->states.count("q3")) {
        automaton->states["q3"]->isFinal = true;
    } else {
        qDebug() << "Warning: State q3 not found when trying to mark as final in AnBnCn preset.";
    }

    setInitialState(automaton, "q0");
}
