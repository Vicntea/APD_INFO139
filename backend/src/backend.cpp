#include <tauri/tauri.hpp>
#include <string>
#include <vector>
#include <cstdlib>
#include <iostream>

// Suponiendo que la ruta a tu ejecutable es "./bin/apd_simulator"
void ejecutar_simulacion(const std::string& input) {
    // Ejecuta el simulador C++ y pasa el input como argumento
    std::string command = "./bin/apd_simulator " + input;
    int result = std::system(command.c_str());

    // Imprimir el resultado del proceso
    if (result == 0) {
        std::cout << "Simulación ejecutada correctamente" << std::endl;
    } else {
        std::cout << "Hubo un error al ejecutar la simulación" << std::endl;
    }
}

// Comando para la interfaz de Tauri
#[tauri::command]
fn ejecutar_simulacion_tauri(input: String) -> String {
    let result = std::process::Command::new("./bin/apd_simulator")
        .arg(input) // Pasar el input al ejecutable
        .output();

    match result {
        Ok(output) => {
            if !output.stdout.is_empty() {
                String::from_utf8_lossy(&output.stdout).to_string()
            } else {
                "Error: No hay salida del simulador.".to_string()
            }
        },
        Err(e) => format!("Error ejecutando el simulador: {}", e),
    }
}

fn main() {
    tauri::Builder::default()
        .invoke_handler(tauri::generate_handler![ejecutar_simulacion_tauri])
        .run(tauri::generate_context!())
        .expect("Error al ejecutar Tauri");
}
