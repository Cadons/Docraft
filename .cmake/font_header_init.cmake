if(NOT DEFINED OUTPUT_FILE)
    message(FATAL_ERROR "OUTPUT_FILE not defined")
endif()

file(WRITE "${OUTPUT_FILE}" "/* Auto-generated fonts header */\n")
file(APPEND "${OUTPUT_FILE}" "#pragma once\n")
file(APPEND "${OUTPUT_FILE}" "#include \"docraft/utils/docraft_font_registry.h\"\n\n")
file(APPEND "${OUTPUT_FILE}" "void docraft_register_fonts() {}\n\n")