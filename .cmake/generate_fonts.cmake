#
# generate_docraft_fonts
#
# Function to generate embedded font resources from a JSON configuration file
#
# Parameters:
#   artifact - Target name to add the generated fonts header to
#   json_config_path - Path to the JSON file containing font configurations
#
function(generate_docraft_fonts artifact json_config_path)

    if(NOT EXISTS "${json_config_path}")
        message(FATAL_ERROR "Fonts config JSON not found: ${json_config_path}")
    endif()

    #
    # Resolve script paths safely
    #
    cmake_path(SET BASE_DIR NORMALIZE "${CMAKE_CURRENT_LIST_DIR}/..")

    set(EMBED_SCRIPT "${BASE_DIR}/.cmake/embed_resource.cmake")
    set(CONCAT_SCRIPT "${BASE_DIR}/.cmake/concat_files.cmake")
    set(FONT_HEADER_INIT_SCRIPT "${BASE_DIR}/.cmake/font_header_init.cmake")

    foreach(SCRIPT
        EMBED_SCRIPT
        CONCAT_SCRIPT
        FONT_HEADER_INIT_SCRIPT
    )
        if(NOT EXISTS "${${SCRIPT}}")
            message(FATAL_ERROR "Missing required script: ${${SCRIPT}}")
        endif()
    endforeach()

    #
    # Read JSON config
    #
    file(READ "${json_config_path}" FONTS_JSON_DATA)

    string(JSON FONTS_COUNT LENGTH "${FONTS_JSON_DATA}")
    math(EXPR FONTS_STOP "${FONTS_COUNT} - 1")

    #
    # Output directories
    #
    set(FONT_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/resources/fonts/incs")
    cmake_path(NORMAL_PATH FONT_OUTPUT_DIR)

    file(MAKE_DIRECTORY "${FONT_OUTPUT_DIR}")

    set(FONT_FRAGMENTS)

    #
    # Font embedding
    #
    foreach(INDEX RANGE ${FONTS_STOP})

        string(JSON FONT_VAR_NAME GET "${FONTS_JSON_DATA}" ${INDEX} "name")
        string(JSON FONT_REL_PATH GET "${FONTS_JSON_DATA}" ${INDEX} "path")

        set(INPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/resources/${FONT_REL_PATH}")
        cmake_path(NORMAL_PATH INPUT_PATH)

        set(OUTPUT_INC "${FONT_OUTPUT_DIR}/${FONT_VAR_NAME}.inc")

        if(NOT EXISTS "${INPUT_PATH}")
            message(WARNING "Font file missing: ${INPUT_PATH}")
            continue()
        endif()
    #perform embedding for this font, the embed script will read the font file, convert it to a byte array and write a header fragment with the variable declaration and initialization
        add_custom_command(
            OUTPUT "${OUTPUT_INC}"

            COMMAND "${CMAKE_COMMAND}" # invoke cmake to run the embedding script
                -DINPUT_FILE=${INPUT_PATH} # path to the font file to embed
                -DOUTPUT_PART=${OUTPUT_INC} # path to the generated header fragment for this font
                -DVARIABLE_NAME=${FONT_VAR_NAME} # name of the variable to generate in the header fragment
                -P "${EMBED_SCRIPT}" # path to the embedding script

            DEPENDS
                "${INPUT_PATH}"
                "${EMBED_SCRIPT}"

            COMMENT "Encoding font bytes: ${FONT_VAR_NAME}"

            VERBATIM
        )

        list(APPEND FONT_FRAGMENTS "${OUTPUT_INC}")#keep track of the generated header fragments for later concatenation

    endforeach()

    #
    # Final header generation
    #
    set(FINAL_FONTS_H "${CMAKE_CURRENT_BINARY_DIR}/fonts.h")
    cmake_path(NORMAL_PATH FINAL_FONTS_H)

    list(JOIN FONT_FRAGMENTS "|" FONT_FRAGMENTS_JOINED)
    #write the final header by first initializing it with the necessary includes and function declaration,
    #then concatenating all the generated fragments into it.
    #The final header will be added as a source to the target artifact and marked as generated.
    add_custom_command(
        OUTPUT "${FINAL_FONTS_H}"

        COMMAND "${CMAKE_COMMAND}" -E rm -f "${FINAL_FONTS_H}"

        COMMAND "${CMAKE_COMMAND}"
            -DOUTPUT_FILE=${FINAL_FONTS_H}
            -P "${FONT_HEADER_INIT_SCRIPT}"

        COMMAND "${CMAKE_COMMAND}"
            -DINPUT_FILES_STR=${FONT_FRAGMENTS_JOINED}
            -DOUTPUT_FILE=${FINAL_FONTS_H}
            -P "${CONCAT_SCRIPT}"

        DEPENDS
            ${FONT_FRAGMENTS}
            "${CONCAT_SCRIPT}"
            "${FONT_HEADER_INIT_SCRIPT}"

        COMMENT "Aggregating fonts into fonts.h"

        VERBATIM
    )

    set_source_files_properties(
        "${FINAL_FONTS_H}"
        PROPERTIES GENERATED TRUE
    )

    target_sources("${artifact}" PRIVATE "${FINAL_FONTS_H}")

    target_include_directories(
        "${artifact}"
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    )

endfunction()
