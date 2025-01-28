cmake_minimum_required(VERSION 3.5)
project(OceanEye VERSION 0.3 LANGUAGES CXX)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

set(COMPONENT_NAME_MAIN OceanEye)
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install)

set(CPACK_GENERATOR "IFW")

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets Concurrent Test)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Concurrent Test)

# set(PYTHON_EXECUTABLE "${CMAKE_SOURCE_DIR}/python3.12-embed/python.exe")
# set(PYTHON_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/python3.12-embed/Include")

set(PYBIND11_TEST OFF)

add_subdirectory(external/pybind11)
set(PYBIND_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/external/pybind11/include)

link_directories(${CPYTHON_LIBRARY_DIR})

if (WIN32)
    # you can put your own install path, or add OpenCV to the global path if you really want
    find_package(OpenCV REQUIRED PATHS "C:/opencv/build")
else ()
    find_package(OpenCV REQUIRED)
endif ()

set(YAML_CPP_BUILD_TESTS OFF)
add_subdirectory(external/yaml-cpp)
set_target_properties(yaml-cpp PROPERTIES COMPILE_FLAGS "-w")
# set(YAML_CPP_BUILD_TESTS OFF)

add_subdirectory(external/QtAwesome)

set(PROJECT_SOURCES
    src/main.cpp

    src/core/mainwindow.cpp src/core/mainwindow.h src/core/mainwindow.ui
    src/core/exportdialog.cpp src/core/exportdialog.h src/core/exportdialog.ui
    src/core/importdialog.cpp src/core/importdialog.h src/core/importdialog.ui
    src/core/editmediadialog.h src/core/editmediadialog.cpp src/core/editmedia.ui
    src/core/welcomewindow.cpp src/core/welcomewindow.h src/core/welcomewindow.ui
    src/core/detectoptions.h src/core/detectoptions.cpp src/core/detectoptions.ui
    
    src/gui/annotatedimage.cpp src/gui/annotatedimage.h
    src/gui/flowlayout.cpp src/gui/flowlayout.h
    src/gui/waitingdialog.cpp src/gui/waitingdialog.h

    src/util/yolov8.cpp src/util/yolov8.h
    src/util/project.cpp src/util/project.h
    src/util/videoslicer.cpp src/util/videoslicer.h

    src/gui/settings/settings.h src/gui/settings/settings.cpp
    src/gui/settings/projectsettings.h src/gui/settings/projectsettings.cpp
    src/gui/settings/globalsettings.h src/gui/settings/globalsettings.cpp
    src/util/settings.h src/util/settings.cpp
    src/util/logger.h src/util/logger.cpp

    src/train/modeltrainer.h src/train/modeltrainer.cpp

    src/resources/resources.qrc
    qdarkstyle/dark/darkstyle.qrc
    qdarkstyle/light/lightstyle.qrc
)

include_directories(${OpenCV_INCLUDE_DIRS} src/util yaml-cpp/include)
if(APPLE)
    execute_process(COMMAND xcrun --sdk macosx --show-sdk-path
                    OUTPUT_VARIABLE CMAKE_OSX_SYSROOT
                    OUTPUT_STRIP_TRAILING_WHITESPACE)
    if(NOT EXISTS "${CMAKE_OSX_SYSROOT}")
        message(FATAL_ERROR "The detected macOS SDK path does not exist: ${CMAKE_OSX_SYSROOT}")
    endif()
endif()

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(OceanEye
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        packages/com.oceaneye.root/package.xml
        src/resources/licence.txt
        src/resources/about.md
        src/core/importdialog.ui
        src/train/modeltrainer.h src/train/modeltrainer.cpp
        src/train/python/custom_yolo.py
        src/train/traindialog.h src/train/traindialog.cpp src/train/traindialog.ui
        src/core/newprojectdialog.h src/core/newprojectdialog.cpp src/core/newprojectdialog.ui

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

target_link_libraries(OceanEye PRIVATE QtAwesome)
target_link_libraries(OceanEye PRIVATE yaml-cpp)
target_link_libraries(OceanEye PRIVATE pybind11::embed)
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



if (WIN32)
    cmake_path(GET CMAKE_C_COMPILER PARENT_PATH MINGW_BIN_DIR)
    cmake_path(GET MINGW_BIN_DIR PARENT_PATH MINGW_DIR)

    install(FILES $<TARGET_RUNTIME_DLLS:OceanEye> TYPE BIN)
    install(FILES ${MINGW_DIR}/x86_64-w64-mingw32/lib/libstdc++-6.dll TYPE BIN)
    install(FILES ${MINGW_DIR}/x86_64-w64-mingw32/lib/libgcc_s_seh-1.dll TYPE BIN)
    install(FILES ${MINGW_DIR}/x86_64-w64-mingw32/lib/libwinpthread-1.dll TYPE BIN)
    install(FILES C:/opencv/build/install/x64/mingw/bin/opencv_videoio_ffmpeg490_64.dll TYPE BIN)
    install(DIRECTORY ${PROJECT_SOURCE_DIR}/external/python3.12-embed/ TYPE BIN)
    install(DIRECTORY ${PROJECT_SOURCE_DIR}/src/train/python TYPE BIN)

    add_custom_command(TARGET OceanEye POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${PROJECT_SOURCE_DIR}/external/python3.12-embed"
            $<TARGET_FILE_DIR:OceanEye>)

    set(CPACK_IFW_ROOT "C:/Qt/Tools/QtInstallerFramework/4.8")

    find_program(WINDEPLOYQT windeployqt HINTS "${_qt_bin_dir}")
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/deploy-qt-windows.cmake.in" "${CMAKE_CURRENT_SOURCE_DIR}/deploy-qt-windows.cmake" @ONLY)

    set(CPACK_PRE_BUILD_SCRIPTS ${CMAKE_CURRENT_SOURCE_DIR}/deploy-qt-windows.cmake)

    set(CPACK_GENERATOR "IFW")
endif ()

if (APPLE)    
    install(CODE "
    include(BundleUtilities)
    fixup_bundle(\"\${CMAKE_INSTALL_PREFIX}/OceanEye.app\" \"\" \"${OpenCV_DIR}\")
    " COMPONENT Runtime)

    # add_custom_command(TARGET OceanEye POST_BUILD
    #     COMMAND ${CMAKE_COMMAND} -E copy_directory
    #     ${CMAKE_SOURCE_DIR}/resources
    #     $<TARGET_FILE_DIR:OceanEye>/../Resources)

    set_target_properties(OceanEye PROPERTIES
            INSTALL_RPATH @executable_path/../Frameworks
            RESOURCE "${SHARED_STATIC_ASSETS}")

    set(CPACK_GENERATOR "DragNDrop")
endif ()

add_custom_command(TARGET OceanEye POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/src/train/python"
        $<TARGET_FILE_DIR:OceanEye>/python)

# TO CREATE INSTALLER RUN COMMANDS IN THIS ORDER
# cmake --build . --target all
# cmake  --build  . --target install
# cmake --build . --target package

# FOR EXAMPLE
# "C:\Program Files\CMake\bin\cmake.exe" --build C:/Users/Ethan/Desktop/OceanEye-QT/build/Desktop_Qt_6_7_2_MinGW_64_bit-MinSizeRel --target all

install(TARGETS OceanEye DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT ${COMPONENT_NAME_MAIN})

include(InstallRequiredSystemLibraries)
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "3")
set(CPACK_MONOLITHIC_INSTALL TRUE)
set(CPACK_PACKAGE_DIRECTORY dist)
set(CPACK_PACKAGE_INSTALL_DIRECTORY ${CPACK_PACKAGE_NAME})
set(CPACK_PACKAGE_HOMEPAGE_URL https://github.com/OceanEye-Project/OceanEye-QT)
set(CPACK_VERBATIM_VARIABLES TRUE)
# set(CPACK_PACKAGE_FILE_NAME ${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_SYSTEM_NAME}-installer)
# set(CPACK_PACKAGE_EXECUTABLES "OceanEye" "OceanEye")
# set(CPACK_CREATE_DESKTOP_LINKS "OceanEye")

set(CPACK_IFW_PACKAGE_NAME "OceanEye")
set(CPACK_IFW_PACKAGE_TITLE "OceanEye")
set(CPACK_IFW_PACKAGE_PUBLISHER "SOLIDS Lab, UVic")
set(CPACK_IFW_PACKAGE_WIZARD_STYLE "Aero")
set(CPACK_IFW_PACKAGE_MAINTENANCE_TOOL_NAME "OceanEye Configuration")
set(CPACK_IFW_PACKAGE_CONTROL_SCRIPT "${CMAKE_SOURCE_DIR}/packages/com.oceaneye.root/installscript.qs")

set (CPACK_IFW_PACKAGE_START_MENU_DIRECTORY "OceanEye")
set (CPACK_IFW_TARGET_DIRECTORY "@ApplicationsDir@/OceanEye")
set (CPACK_IFW_ADMIN_TARGET_DIRECTORY "@ApplicationsDir@/OceanEye")

# set(CPACK_IFW_PACKAGE_RUN_PROGRAM "@TargetDir@/OceanEye")
# set(CPACK_IFW_PACKAGE_RUN_PROGRAM_DESCRIPTION "Start OceanEye")
# set(CPACK_IFW_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/ScreenPlay/assets/icons/app.ico")
# set(CPACK_IFW_PACKAGE_WINDOW_ICON "${CMAKE_SOURCE_DIR}/ScreenPlay/assets/icons/app.ico")

include(CPACK)
include(CPackIFW)

CPACK_ADD_COMPONENT(${COMPONENT_NAME_MAIN} REQUIRED)
cpack_ifw_configure_component(${COMPONENT_NAME_MAIN} ESSENTIAL FORCED_INSTALLATION)
