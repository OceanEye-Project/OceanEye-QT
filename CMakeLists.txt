cmake_minimum_required(VERSION 3.5)

set(CMAKE_TOOLCHAIN_FILE $ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)

# set(CMAKE_C_COMPILER "gcc")
# set(CMAKE_CXX_COMPILER "g++")
# set(CMAKE_MAKE_PROGRAM "ninja")

project(OceanEye VERSION 0.2 LANGUAGES CXX)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")

set(COMPONENT_NAME_MAIN OceanEye)
set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install)

set(CPACK_GENERATOR "IFW")

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Concurrent)
find_package(OpenCV REQUIRED)
find_package(yaml-cpp REQUIRED)

set(PROJECT_SOURCES
    src/main.cpp

    src/core/mainwindow.cpp src/core/mainwindow.h src/core/mainwindow.ui
    src/core/exportdialog.cpp src/core/exportdialog.h src/core/exportdialog.ui
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
    packages/com.oceaneye.root/package.xml
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
    install(FILES $<TARGET_RUNTIME_DLLS:OceanEye> TYPE BIN)

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

# TO CREATE INSTALLER RUN COMMANDS IN THIS ORDER
# cmake --build . --target all
# cmake  --build  . --target install
# cmake --build . --target package

# FOR EXAMPLE
# "C:\Program Files\CMake\bin\cmake.exe" --build C:/Users/Ethan/Desktop/OceanEye-QT/build/Desktop_Qt_6_7_2_MinGW_64_bit-MinSizeRel --target all
# Removed duplicate install(TARGETS OceanEye ...) to avoid install errors
install(TARGETS OceanEye DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT ${COMPONENT_NAME_MAIN})

include(InstallRequiredSystemLibraries)
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "2")
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
