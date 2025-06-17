set(WINDEPLOYQT "C:/Users/Ethan/Desktop/OceanEye-QT/build/vcpkg_installed/x64-mingw-static/tools/Qt6/bin/windeployqt.exe")
set(COMPONENT_NAME_MAIN "OceanEye")
set(CMAKE_CURRENT_SOURCE_DIR "C:/Users/Ethan/Desktop/OceanEye-QT")

execute_process(COMMAND ${WINDEPLOYQT} --qmldir ${CMAKE_CURRENT_SOURCE_DIR} ${COMPONENT_NAME_MAIN}/data/bin WORKING_DIRECTORY ${CPACK_TEMPORARY_INSTALL_DIRECTORY}/packages)
