cmake_minimum_required(VERSION 3.5)

project(OceanEyeTest VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Concurrent Test)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Concurrent Test)

find_package(OpenCV REQUIRED)

set(PROJECT_SOURCES
    test_welcomewindow.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(OceanEyeTest
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ../mainwindow.cpp
        ../mainwindow.h
        ../mainwindow.ui
        ../welcomewindow.cpp
        ../welcomewindow.h
        ../welcomewindow.ui
        ../project.h ../project.cpp
        ../resources.qrc
        ../yolov8.h ../yolov8.cpp
        ../annotatedimage.h ../annotatedimage.cpp
        ../styles.qss
        ../exportdialog.h ../exportdialog.cpp
        ../exportdialog.ui

    )
else()
    if(ANDROID)
        add_library(OceanEyeTest SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(OceanEyeTest
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(OceanEyeTest PRIVATE ${OpenCV_LIBS})
target_link_libraries(OceanEyeTest PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(OceanEyeTest PRIVATE Qt${QT_VERSION_MAJOR}::Concurrent)
target_link_libraries(OceanEyeTest PRIVATE Qt${QT_VERSION_MAJOR}::Test)

if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.OceanEyeTest)
endif()
set_target_properties(OceanEyeTest PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS OceanEyeTest
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(OceanEyeTest)
endif()
