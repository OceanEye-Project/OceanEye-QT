cmake_minimum_required(VERSION 3.5)

project(OceanEyeWelcomeWindowTest VERSION 0.1 LANGUAGES CXX)
project(OceanEyeMainWindowTest VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Concurrent Test)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Concurrent Test)

find_package(OpenCV REQUIRED)

set(WELCOME_WINDOW
    test_welcomewindow.cpp
)

set(MAIN_WINDOW_TESTS
    test_mainwindow.cpp
)

set(MAIN
    ../src/main.cpp
)

set(ALL_FILES
    ../src/core/mainwindow.cpp ../src/core/mainwindow.h ../src/core/mainwindow.ui
    ../src/core/exportdialog.cpp ../src/core/exportdialog.h ../src/core/exportdialog.ui
    ../src/core/editmediadialog.h ../src/core/editmediadialog.cpp ../src/core/editmedia.ui
    ../src/core/welcomewindow.cpp ../src/core/welcomewindow.h ../src/core/welcomewindow.ui
    ../src/core/detectoptions.h ../src/core/detectoptions.cpp ../src/core/detectoptions.ui

    ../src/gui/annotatedimage.cpp ../src/gui/annotatedimage.h
    ../src/gui/flowlayout.cpp ../src/gui/flowlayout.h
    ../src/gui/waitingdialog.cpp ../src/gui/waitingdialog.h

    ../src/util/yolov8.cpp ../src/util/yolov8.h
    ../src/util/project.cpp ../src/util/project.h
    ../src/util/videoslicer.cpp ../src/util/videoslicer.h

    ../src/gui/settings/settings.h ../src/gui/settings/settings.cpp
    ../src/gui/settings/projectsettings.h ../src/gui/settings/projectsettings.cpp
    ../src/gui/settings/globalsettings.h ../src/gui/settings/globalsettings.cpp
    ../src/util/settings.h

    ../src/resources/resources.qrc
    ../qdarkstyle/dark/darkstyle.qrc
    ../qdarkstyle/light/lightstyle.qrc
)

if(APPLE)
    execute_process(COMMAND xcrun --sdk macosx --show-sdk-path
                    OUTPUT_VARIABLE CMAKE_OSX_SYSROOT
                    OUTPUT_STRIP_TRAILING_WHITESPACE)
    if(NOT EXISTS "${CMAKE_OSX_SYSROOT}")
        message(FATAL_ERROR "The detected macOS SDK path does not exist: ${CMAKE_OSX_SYSROOT}")
    endif()
endif()

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(OceanEyeWelcomeWindowTest
        MANUAL_FINALIZATION
        ${WELCOME_WINDOW}
        ${ALL_FILES}
    )
    qt_add_executable(OceanEyeMainWindowTest
        MANUAL_FINALIZATION
        ${MAIN_WINDOW_TESTS}
        ${ALL_FILES}
    )
else()
    if(ANDROID)
        add_library(OceanEyeWelcomeWindowTest SHARED
            ${WELCOME_WINDOW}
        )
        add_library(OceanEyeMainWindowTest SHARED
            ${MAIN_WINDOW_TESTS}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(OceanEyeWelcomeWindowTest
            ${WELCOME_WINDOW}
        )
        add_executable(OceanEyeMainWindowTest
            ${MAIN_WINDOW_TESTS}
        )
    endif()
endif()

target_link_libraries(OceanEyeWelcomeWindowTest PRIVATE ${OpenCV_LIBS})
target_link_libraries(OceanEyeWelcomeWindowTest PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(OceanEyeWelcomeWindowTest PRIVATE Qt${QT_VERSION_MAJOR}::Concurrent)
target_link_libraries(OceanEyeWelcomeWindowTest PRIVATE Qt${QT_VERSION_MAJOR}::Test)

target_link_libraries(OceanEyeMainWindowTest PRIVATE ${OpenCV_LIBS})
target_link_libraries(OceanEyeMainWindowTest PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(OceanEyeMainWindowTest PRIVATE Qt${QT_VERSION_MAJOR}::Concurrent)
target_link_libraries(OceanEyeMainWindowTest PRIVATE Qt${QT_VERSION_MAJOR}::Test)

if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.OceanEyeWelcomeWindowTest)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.OceanEyeMainWindowTest)
endif()
set_target_properties(OceanEyeWelcomeWindowTest PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)
set_target_properties(OceanEyeMainWindowTest PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS OceanEyeWelcomeWindowTest
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
install(TARGETS OceanEyeMainWindowTest
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(OceanEyeWelcomeWindowTest)
endif()

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(OceanEyeMainWindowTest)
endif()