if(NOT DEFINED OUTPUT_FILE)
    message(FATAL_ERROR "OUTPUT_FILE not defined")
endif()

# fonts.h's static font-registration initializers only run if the header is compiled as
# part of a real translation unit -- a header merely listed as a CMake target source is
# never fed to the compiler. This tiny .cc exists solely to include it.
file(WRITE "${OUTPUT_FILE}" "// Auto-generated: compiles fonts.h as a translation unit so its\n")
file(APPEND "${OUTPUT_FILE}" "// static font-registration initializers actually run.\n")
file(APPEND "${OUTPUT_FILE}" "#include \"fonts.h\"\n")
