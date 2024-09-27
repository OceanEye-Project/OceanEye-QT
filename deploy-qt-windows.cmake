set(WINDEPLOYQT "C:/Qt/6.7.2/mingw_64/bin/windeployqt.exe")
set(COMPONENT_NAME_MAIN "OceanEye")
set(CMAKE_CURRENT_SOURCE_DIR "C:/Users/arden/Documents/QTProjects/OceanEye-QT")

execute_process(COMMAND ${WINDEPLOYQT} --qmldir ${CMAKE_CURRENT_SOURCE_DIR} ${COMPONENT_NAME_MAIN}/data/bin WORKING_DIRECTORY ${CPACK_TEMPORARY_INSTALL_DIRECTORY}/packages)
