#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton> // Ya incluido, pero lo dejo para que quede claro si lo usas directamente
#include <QLineEdit>   // Ya incluido
#include <QListWidget> // Ya incluido
#include <QComboBox>   // ¡NUEVO! Necesario para el selector de presets
#include <string>
#include <vector>

// Inclusiones de tu backend.
// Como 'include_directories(../backend/include)' está configurado,
// puedes incluirlos directamente por su nombre.
#include "Automaton.hpp"
#include "Stack.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_evaluateButton_clicked();
    void on_addTransitionButton_clicked();
    void on_resetButton_clicked();
    void on_addFinalStateButton_clicked();
    void on_initialStateLineEdit_textEdited(const QString &text);

    // --- ¡NUEVO SLOT para el QComboBox de presets! ---
    // Este slot se activará cuando el usuario cambie la selección en el ComboBox.
    void on_presetComboBox_currentIndexChanged(int index);

private:
    Ui::MainWindow *ui;

    Stack *pila;
    Automaton *automaton;

    // --- Funciones auxiliares existentes ---
    bool parseTransitionString(const std::string& transitionStr, std::string& q_src, char& sym_in, char& sym_top, std::string& q_dest, std::string& push_str);
    void updateTransitionsList();
    void updateFinalStatesList();
    std::vector<std::string> user_defined_transitions; // Para las transiciones añadidas manualmente.

    // --- ¡NUEVAS FUNCIONES para la gestión de presets! ---
    // Esta función auxiliar se encargará de limpiar el autómata y la pila actuales.
    void clearAndResetAutomaton();
    // Estas funciones cargarán los diferentes autómatas predefinidos.
    void loadEmptyAutomaton();     // Carga un autómata vacío (estado inicial del programa).
    void loadAnBnAutomaton();      // Carga el autómata para a^n b^n.
    void loadAnBnCnAutomaton();    // Carga el autómata para a^n b^n c^n (o cualquier otro preset).
};
#endif // MAINWINDOW_H
