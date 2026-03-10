cmake_minimum_required(VERSION 3.16)
option(BUILD_TESTS "Build tests" ON)
# Resolve project/library version from project.json
set(DOCRAFT_PROJECT_JSON "${CMAKE_CURRENT_SOURCE_DIR}/project.json")
if(NOT EXISTS "${DOCRAFT_PROJECT_JSON}")
    message(FATAL_ERROR "Missing ${DOCRAFT_PROJECT_JSON}")
endif()

file(READ "${DOCRAFT_PROJECT_JSON}" DOCRAFT_PROJECT_JSON_CONTENT)
string(JSON DOCRAFT_LIBRARY_VERSION
        ERROR_VARIABLE DOCRAFT_LIBRARY_VERSION_ERROR
        GET "${DOCRAFT_PROJECT_JSON_CONTENT}" version
)
if(DOCRAFT_LIBRARY_VERSION_ERROR)
    message(FATAL_ERROR "Unable to read \"version\" from ${DOCRAFT_PROJECT_JSON}: ${DOCRAFT_LIBRARY_VERSION_ERROR}")
endif()
string(REGEX MATCH "^[0-9]+" DOCRAFT_LIBRARY_SOVERSION "${DOCRAFT_LIBRARY_VERSION}")
if(DOCRAFT_LIBRARY_SOVERSION STREQUAL "")
    message(FATAL_ERROR "Invalid version \"${DOCRAFT_LIBRARY_VERSION}\" in ${DOCRAFT_PROJECT_JSON}")
endif()

# Project name and compiler
project(docraft VERSION ${DOCRAFT_LIBRARY_VERSION} LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Set C++ standard (change if you need another version)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_SHARED_LIB "Build docraft as a shared library" OFF)

include(.cmake/utils.cmake)
configure_build_folder(artifacts)
#find packages
#....
find_package(pugixml CONFIG REQUIRED)
find_package(unofficial-libharu QUIET)
find_package(nlohmann_json CONFIG REQUIRED)
if(unofficial-libharu_FOUND)
    set(LIBHARU_TARGET unofficial::libharu::hpdf)
else()
    find_library(LIBHARU_LIBRARY NAMES hpdf libhpdf)
    find_path(LIBHARU_INCLUDE_DIR NAMES hpdf.h)

    if(LIBHARU_LIBRARY AND LIBHARU_INCLUDE_DIR)
        add_library(libharu UNKNOWN IMPORTED)
        set_target_properties(libharu PROPERTIES
                IMPORTED_LOCATION "${LIBHARU_LIBRARY}"
                INTERFACE_INCLUDE_DIRECTORIES "${LIBHARU_INCLUDE_DIR}"
        )
        set(LIBHARU_TARGET libharu)
    else()
        message(FATAL_ERROR "libharu not found. Please install libharu or unofficial-libharu package.")
    endif()
endif()

set(DOCRAFT_INSTALL_CMAKEDIR "${CMAKE_INSTALL_DATADIR}/docraft")

#artifacts
add_subdirectory(docraft) #configure docraft artifact

configure_package_config_file(
        "${CMAKE_CURRENT_LIST_DIR}/cmake/DocraftConfig.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/docraftConfig.cmake"
        INSTALL_DESTINATION "${DOCRAFT_INSTALL_CMAKEDIR}"
)

write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/docraftConfigVersion.cmake"
        VERSION "${PROJECT_VERSION}"
        COMPATIBILITY SameMajorVersion
)

install(
        FILES
        "${CMAKE_CURRENT_BINARY_DIR}/docraftConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/docraftConfigVersion.cmake"
        DESTINATION "${DOCRAFT_INSTALL_CMAKEDIR}"
)

#test
include(CTest)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(docraft/test) #configure tests artifact in test
endif()
