cmake_minimum_required(VERSION 3.5)

project(RegistrationGenerator VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# ---- источники ----
# ...
set(PROJECT_SOURCES
    src/main.cpp
    src/mainwindow/mainwindow.cpp
    src/mainwindow/mainwindow.h
    src/mainwindow/mainwindow.ui
    src/generator/generator.cpp
    src/generator/generator.h
    src/structures/structures.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(RegistrationGenerator
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(RegistrationGenerator SHARED ${PROJECT_SOURCES})
    else()
        add_executable(RegistrationGenerator ${PROJECT_SOURCES})
    endif()
endif()

# ---- заголовочные пути для всех подпапок src ----
target_include_directories(RegistrationGenerator PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/mainwindow
    ${CMAKE_SOURCE_DIR}/src/generator
    ${CMAKE_SOURCE_DIR}/src/structures
    ${CMAKE_SOURCE_DIR}/src/varstorage
)

target_link_libraries(RegistrationGenerator PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.RegistrationGenerator)
endif()
set_target_properties(RegistrationGenerator PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS RegistrationGenerator
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(RegistrationGenerator)
endif()