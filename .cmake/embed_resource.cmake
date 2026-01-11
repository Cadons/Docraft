file(READ "${INPUT_FILE}" hex_content HEX)
string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1, " c_content "${hex_content}")

set(VARIABLE_CPP_NAME "${VARIABLE_NAME}")
string(REPLACE "-" "_" VARIABLE_CPP_NAME "${VARIABLE_CPP_NAME}")
file(WRITE "${OUTPUT_PART}" "namespace {\n")
file(APPEND "${OUTPUT_PART}" "  const unsigned char ${VARIABLE_CPP_NAME}_data[] = {\n${c_content}\n  };\n")
file(APPEND "${OUTPUT_PART}" "}\n")
file(APPEND "${OUTPUT_PART}" "static bool _reg_${VARIABLE_CPP_NAME} = docraft::utils::DocraftFontRegistry::instance().register_font(\"${VARIABLE_NAME}\", ${VARIABLE_CPP_NAME}_data, sizeof(${VARIABLE_CPP_NAME}_data));\n\n")