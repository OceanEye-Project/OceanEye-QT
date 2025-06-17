cmake_minimum_required(VERSION 3.5)

set(CMAKE_TOOLCHAIN_FILE $ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")

set(COMPONENT_NAME_MAIN OceanEye)
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install)

project(OceanEye VERSION 0.5 LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Concurrent)
find_package(OpenCV REQUIRED)
find_package(yaml-cpp REQUIRED)

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

    src/YOLO/yolov8.cpp src/YOLO/yolov8.h

    src/util/project.cpp src/util/project.h
    src/util/videoslicer.cpp src/util/videoslicer.h

    src/gui/settings/settings.h src/gui/settings/settings.cpp
    src/gui/settings/projectsettings.h src/gui/settings/projectsettings.cpp
    src/gui/settings/globalsettings.h src/gui/settings/globalsettings.cpp
    src/util/settings.h src/util/settings.cpp
    src/util/logger.h src/util/logger.cpp

    src/core/importdialog.ui
    src/train/modeltrainer.h src/train/modeltrainer.cpp
    src/train/python/custom_yolo.py
    src/train/traindialog.h src/train/traindialog.cpp src/train/traindialog.ui
    src/core/newprojectdialog.h src/core/newprojectdialog.cpp src/core/newprojectdialog.ui

    src/resources/resources.qrc
)

set(RESOURCE_FILES
    src/resources/licence.txt
    src/resources/about.md
    src/resources/logo.rc
)

qt_add_executable(OceanEye
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
    ${RESOURCE_FILES}
)

target_link_libraries(OceanEye PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Concurrent)
target_link_libraries(OceanEye PRIVATE yaml-cpp::yaml-cpp)
target_link_libraries(OceanEye PRIVATE ${OpenCV_LIBS})

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
    COMPONENT ${COMPONENT_NAME_MAIN}
)

qt_finalize_executable(OceanEye)

if (WIN32)
    # cmake_path(GET CMAKE_C_COMPILER PARENT_PATH MINGW_BIN_DIR)
    # cmake_path(GET MINGW_BIN_DIR PARENT_PATH MINGW_DIR)

    # install(FILES $<TARGET_RUNTIME_DLLS:OceanEye> TYPE BIN)
    # install(DIRECTORY ${PROJECT_SOURCE_DIR}/external/python3.12-embed/ TYPE BIN)
    # install(DIRECTORY ${PROJECT_SOURCE_DIR}/src/train/python TYPE BIN)

    add_custom_command(TARGET OceanEye POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${PROJECT_SOURCE_DIR}/external/python3.12-embed"
            $<TARGET_FILE_DIR:OceanEye>/python)

    add_custom_command(TARGET OceanEye POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${PROJECT_SOURCE_DIR}/src/train/python"
            $<TARGET_FILE_DIR:OceanEye>/python_scripts)

    set(CPACK_IFW_ROOT "C:/Qt/Tools/QtInstallerFramework/4.8")

    find_program(WINDEPLOYQT windeployqt HINTS "${_qt_bin_dir}")
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/installer/deploy-qt-windows.cmake.in" "${CMAKE_CURRENT_SOURCE_DIR}/installer/deploy-qt-windows.cmake" @ONLY)

    set(CPACK_PRE_BUILD_SCRIPTS ${CMAKE_CURRENT_SOURCE_DIR}/installer/deploy-qt-windows.cmake)
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

# TO CREATE INSTALLER RUN COMMANDS IN THIS ORDER
# cmake --build . --target all
# cmake  --build  . --target install
# cmake --build . --target package

# FOR EXAMPLE
# "C:\Program Files\CMake\bin\cmake.exe" --build C:/Users/Ethan/Desktop/OceanEye-QT/build/Desktop_Qt_6_7_2_MinGW_64_bit-MinSizeRel --target all
# Removed duplicate install(TARGETS OceanEye ...) to avoid install errors

