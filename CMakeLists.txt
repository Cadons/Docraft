cmake_minimum_required(VERSION 3.16)

# Project name and compiler
project(DocCraft LANGUAGES CXX)

# Set C++ standard (change if you need another version)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
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

#artifacts
add_subdirectory(docraft) #configure docraft artifact

#test
include(CTest)
enable_testing()
add_subdirectory(docraft/test) #configure tests artifact in test

