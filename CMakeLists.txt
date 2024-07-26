cmake_minimum_required(VERSION 3.5)

project(OceanEye VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Concurrent Test)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Concurrent Test)

find_package(OpenCV REQUIRED)

set(PROJECT_SOURCES
    src/main.cpp

    src/core/mainwindow.cpp src/core/mainwindow.h src/core/mainwindow.ui
    src/core/exportdialog.cpp src/core/exportdialog.h src/core/exportdialog.ui
    src/core/editmediadialog.h src/core/editmediadialog.cpp src/core/editmedia.ui
    src/core/welcomewindow.cpp src/core/welcomewindow.h src/core/welcomewindow.ui
    
    src/gui/annotatedimage.cpp src/gui/annotatedimage.h
    src/gui/flowlayout.cpp src/gui/flowlayout.h
    src/gui/waitingdialog.cpp src/gui/waitingdialog.h

    src/util/yolov8.cpp src/util/yolov8.h
    src/util/project.cpp src/util/project.h
    src/util/videoslicer.cpp src/util/videoslicer.h

    src/resources/resources.qrc
    qdarkstyle/dark/darkstyle.qrc
    qdarkstyle/light/lightstyle.qrc
)


include_directories(${OpenCV_INCLUDE_DIRS})

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(OceanEye
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
    
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET OceanEye APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(OceanEye SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(OceanEye
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(OceanEye PRIVATE ${OpenCV_LIBS})
target_link_libraries(OceanEye PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(OceanEye PRIVATE Qt${QT_VERSION_MAJOR}::Concurrent)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.OceanEye)
endif()
set_target_properties(OceanEye PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS OceanEye
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(OceanEye)
endif()
