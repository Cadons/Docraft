string(REPLACE "\"" "" CLEAN_STR "${INPUT_FILES_STR}")

string(REPLACE "|" ";" FILE_LIST "${CLEAN_STR}")

foreach(FILE IN LISTS FILE_LIST)
    string(STRIP "${FILE}" FILE_TRIMMED)

    if(EXISTS "${FILE_TRIMMED}")
        file(READ "${FILE_TRIMMED}" CONTENT)
        file(APPEND "${OUTPUT_FILE}" "${CONTENT}")
    else()
        message(WARNING "Fragment file not found: [${FILE_TRIMMED}]")
    endif()
endforeach()