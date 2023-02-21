cmake_minimum_required(VERSION 3.5)

project(TACIoT_UI VERSION 0.1 LANGUAGES CXX)
include_directories(../client/)
include_directories(../utils/)
include_directories(../)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        register_ue_screen.h
        register_ue_screen.cpp
        register_ue_screen.ui
        register_ap_screen.h
        register_ap_screen.cpp
        register_ap_screen.ui
        publish_screen.h
        publish_screen.cpp
        publish_screen.ui
        query_screen.h
        query_screen.cpp
        query_screen.ui
        result_screen.h
        result_screen.cpp
        result_screen.ui
        queried_table_screen.h
        queried_table_screen.cpp
        queried_table_screen.ui
        perms_screen.h
        perms_screen.cpp
        perms_screen.ui
        ../../core/utils/errors.h
        ../../core/utils/errors.cpp
        ../client/client_generic_interface.h
        ../client/client_generic_interface.cpp
        ../../config_macros.h
        Resources.qrc
)


if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(TACIoT_UI
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET TACIoT_UI APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(TACIoT_UI SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(TACIoT_UI
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(TACIoT_UI PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

set_target_properties(TACIoT_UI PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS TACIoT_UI
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(TACIoT_UI)
endif()
