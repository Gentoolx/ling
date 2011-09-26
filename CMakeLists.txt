cmake_minimum_required(VERSION 2.8)
project(WntrLing)

set(WNTRLING_MAJOR_VERSION 0)
set(WNTRLING_MINOR_VERSION 0)
set(WNTRLING_MICRO_VERSION 1)
set(WNTRLING_VERSION
    ${WNTRLING_MAJOR_VERSION}.${WNTRLING_MINOR_VERSION}${WNTRLING_MICRO_VERSION})

include(WintermuteMacros)
include(WintermuteDefaults)

## Packages
find_package(WntrData REQUIRED)
find_package(PythonLibs REQUIRED)
find_package(Boost 1.42 COMPONENTS
    python
    REQUIRED)
find_package(Qt4 4.6.3 COMPONENTS
    QtCore
    QtDBus
    QT_DONT_USE_GUI
    REQUIRED)
enable_doxygen()

## Includes
include("${QT_USE_FILE}")

set(WNTRLING_INCLUDE_DIR "${WINTER_INCLUDE_INSTALL_DIR}/ling" CACHE PATH "Includes directory.")
set(WNTRLING_LIB_DIR "${WINTER_LIB_INSTALL_DIR}" CACHE PATH "Libraries directory.")

set(WNTRLING_INCLUDE_DIRS
    ${WNTRLING_INCLUDE_DIR}
    ${WNTRDATA_INCLUDE_DIRS}
    ${PYTHON_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    ${QT_INCLUDE_DIRS})

set(WNTRLING_CMAKE_DIR "${CMAKE_INSTALL_PREFIX}/share/cmake/WntrLing" CACHE PATH "CMake directory.")

set(WNTRLING_LIBRARIES 
    ${WNTRDATA_LIBRARIES}
    ${PYTHON_LIBRARIES}
    ${QT_LIBRARIES}
    ${Boost_LIBRARIES})

include_directories(${WNTRLING_INCLUDE_DIRS})

## Configs
configure_file("cmake/WntrLingConfig.cmake.in"
    "${PROJECT_BINARY_DIR}/InstallFiles/WntrLingConfig.cmake" @ONLY)

configure_file("cmake/WntrLingConfigVersion.cmake.in"
    "${PROJECT_BINARY_DIR}/InstallFiles/WntrLingConfigVersion.cmake" @ONLY)

configure_file("${PROJECT_SOURCE_DIR}/wntrling.pc.in"
   "${PROJECT_BINARY_DIR}/InstallFiles/wntrling.pc" @ONLY)

add_subdirectory(src)

## Installs
install(FILES
    "${PROJECT_BINARY_DIR}/InstallFiles/WntrLingConfig.cmake"
    "${PROJECT_BINARY_DIR}/InstallFiles/WntrLingConfigVersion.cmake"
    DESTINATION "${WNTRLING_CMAKE_DIR}")

install(FILES "${PROJECT_BINARY_DIR}/InstallFiles/wntrling.pc"
    DESTINATION "${WNTRLING_LIB_DIR}/pkgconfig/")
