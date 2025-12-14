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
#lexer and bison

#artifacts
add_subdirectory(docraft) #configure docraft artifact

#test
include(CTest)
enable_testing()
add_subdirectory(docraft/test) #configure tests artifact in test


