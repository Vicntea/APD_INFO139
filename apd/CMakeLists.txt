cmake_minimum_required(VERSION 3.5) # Dejamos 3.5 como lo tenías, aunque 3.16+ es recomendado.

project(apd VERSION 0.1 LANGUAGES CXX) # El nombre del proyecto es 'apd'

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17) # ¡Cambiado a 17! Es importante para la compatibilidad moderna de C++.
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

# --- INICIO DE LA SECCION IMPORTANTE DE INCLUSION DE BACKEND Y UI ---

# 1. Añadir la ruta de inclusión para los headers de tu backend
# Desde la carpeta del proyecto (apd), la ruta a backend/include es "../backend/include"
# ya que 'apd' y 'backend' están al mismo nivel dentro de '~/cpp/'.
include_directories(../backend/include)

# 2. Definir TODAS las fuentes y archivos UI del proyecto
set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h # ¡Mantenemos mainwindow.h para que se vea en el IDE!
    mainwindow.ui # ¡Mantenemos mainwindow.ui para que se vea en el IDE!

    # Archivos fuente de tu backend.
    # Rutas relativas desde el directorio del proyecto (apd).
    ../backend/src/Automaton.cpp
    ../backend/src/Stack.cpp
    # Si tienes otros .cpp en backend/src, añádelos aquí.
    # Por ejemplo: ../backend/src/backend.cpp
)

# --- FIN DE LA SECCION IMPORTANTE DE INCLUSION DE BACKEND Y UI ---

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(apd # El nombre del ejecutable es 'apd'
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(apd SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(apd # El nombre del ejecutable es 'apd'
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(apd PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

set_target_properties(apd PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(apd)
endif()
