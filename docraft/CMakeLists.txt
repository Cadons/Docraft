function(generate_docraft_fonts artifact json_config_path)
    if(NOT EXISTS "${json_config_path}")
        message(FATAL_ERROR "Fonts config JSON not found at: ${json_config_path}")
    endif()
    file(READ "${json_config_path}" FONTS_JSON_DATA)

    string(JSON FONTS_COUNT LENGTH "${FONTS_JSON_DATA}")
    math(EXPR FONTS_STOP "${FONTS_COUNT} - 1")

    set(FONT_FRAGMENTS "")
    set(EMBED_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/../.cmake/embed_resource.cmake")
    set(CONCAT_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/../.cmake/concat_files.cmake")

    foreach(INDEX RANGE ${FONTS_STOP})
        string(JSON FONT_VAR_NAME GET "${FONTS_JSON_DATA}" ${INDEX} "name")
        #upper case
        string(JSON FONT_REL_PATH GET "${FONTS_JSON_DATA}" ${INDEX} "path")

        set(INPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/resources/${FONT_REL_PATH}")
        set(OUTPUT_INC "${CMAKE_CURRENT_BINARY_DIR}/resources/fonts/incs/${FONT_VAR_NAME}.inc")

        if(NOT EXISTS "${INPUT_PATH}")
            message(WARNING "Font file not found: ${INPUT_PATH}. Skipping ${FONT_VAR_NAME}")
            continue()
        endif()

        add_custom_command(
                OUTPUT ${OUTPUT_INC}
                COMMAND ${CMAKE_COMMAND} -DINPUT_FILE=${INPUT_PATH}
                -DOUTPUT_PART=${OUTPUT_INC}
                -DVARIABLE_NAME=${FONT_VAR_NAME}
                -P "${EMBED_SCRIPT}"
                DEPENDS ${INPUT_PATH} "${EMBED_SCRIPT}"
                COMMENT "Encoding font bytes: ${FONT_VAR_NAME}"
                VERBATIM
        )
        list(APPEND FONT_FRAGMENTS ${OUTPUT_INC})
    endforeach()

    set(FINAL_FONTS_H "${CMAKE_CURRENT_BINARY_DIR}/fonts.h")
    string(REPLACE ";" "|" FONT_FRAGMENTS_JOINED "${FONT_FRAGMENTS}")
    add_custom_command(
            OUTPUT ${FINAL_FONTS_H}
            COMMAND ${CMAKE_COMMAND} -E echo "/* Auto-generated fonts header */" > ${FINAL_FONTS_H}
            COMMAND ${CMAKE_COMMAND} -E echo "#pragma once" >> ${FINAL_FONTS_H}
            COMMAND ${CMAKE_COMMAND} -E echo "#include \"utils/docraft_font_registry.h\"" >> ${FINAL_FONTS_H}
            COMMAND ${CMAKE_COMMAND} -E echo "void docraft_register_fonts() {}" >> ${FINAL_FONTS_H}
            COMMAND ${CMAKE_COMMAND} -DINPUT_FILES_STR="${FONT_FRAGMENTS_JOINED}"
            -DOUTPUT_FILE=${FINAL_FONTS_H}
            -P "${CONCAT_SCRIPT}"
            DEPENDS ${FONT_FRAGMENTS} "${CONCAT_SCRIPT}"
            COMMENT "Aggregating all fonts into fonts.h"
            VERBATIM
    )

    set_source_files_properties(${FINAL_FONTS_H} PROPERTIES GENERATED TRUE)
    target_sources(${artifact} PRIVATE ${FINAL_FONTS_H})
    target_include_directories(${artifact}  PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
endfunction()

set(DOCRAFT_SOURCES src/main.cpp
        src/model/docraft_node.cc
        include/model/docraft_node.h
        src/model/docraft_section.cc
        include/model/docraft_section.h
        src/model/docraft_body.cc
        include/model/docraft_body.h
        src/model/docraft_text.cc
        include/model/docraft_text.h
        src/docraft_document.cc
        include/docraft_document.h
        src/docraft_pdf_context.cc
        include/docraft_pdf_context.h
        src/docraft_cursor.cc
        include/docraft_cursor.h
        src/docraft_color.cc
        include/docraft_color.h
        include/renderer/docraft_renderer.h
        src/renderer/docraft_pdf_renderer.cc
        include/renderer/docraft_pdf_renderer.h
        src/renderer/painter/docraft_text_painter.cc
        include/renderer/painter/docraft_text_painter.h
        include/renderer/painter/i_painter.h
        src/layout/docraft_layout_engine.cc
        include/layout/docraft_layout_engine.h
        src/layout/handler/docraft_layout_text_handler.cc
        include/layout/handler/docraft_layout_text_handler.h
        include/generic/chain_of_responsibility_handler.h
        include/layout/handler/abstract_docraft_layout_handler.h
        src/layout/handler/docraft_layout_section_handler.cc
        include/layout/handler/docraft_layout_section_handler.h
        src/generic/docraft_font_applier.cc
        include/generic/docraft_font_applier.h
        src/model/docraft_header.cc
        include/model/docraft_header.h
        src/model/docraft_paragraph.cc
        include/model/docraft_paragraph.h
        src/model/docraft_layout.cc
        include/model/docraft_layout.h
        src/layout/handler/docraft_layout_handler.cc
        include/layout/handler/docraft_layout_handler.h
        src/model/docraft_children_container_node.cc
        include/model/docraft_children_container_node.h
        include/backend/docraft_pdf_backend.h
        src/model/docraft_image.cc
        include/model/docraft_image.h
        src/renderer/painter/docraft_image_painter.cc
        include/renderer/painter/docraft_image_painter.h
        src/layout/handler/docraft_basic_layout_handler.cc
        include/layout/handler/docraft_basic_layout_handler.h
        src/model/docraft_footer.cc
        include/model/docraft_footer.h
        src/model/docraft_table.cc
        include/model/docraft_table.h
        src/renderer/painter/docraft_table_painter.cc
        include/renderer/painter/docraft_table_painter.h
        src/layout/handler/docraft_layout_table_handler.cc
        include/layout/handler/docraft_layout_table_handler.h
        include/utils/docraft_font_registry.h
        src/model/docraft_rectangle.cc
        include/model/docraft_rectangle.h
        src/renderer/painter/docraft_rectangle_painter.cc
        include/renderer/painter/docraft_rectangle_painter.h
        src/model/docraft_blank_line.cc
        include/model/docraft_blank_line.h
        src/layout/handler/docraft_layout_blank_line.cc
        include/layout/handler/docraft_layout_blank_line.h
        src/renderer/painter/docraft_blank_line_painter.cc
        include/renderer/painter/docraft_blank_line_painter.h
        src/craft/docraft_craft_language_parser.cc
        include/craft/docraft_craft_language_parser.h
        include/craft/i_docraft_parser.h
        src/craft/parser/docraft_parser.cc
        include/craft/parser/docraft_parser.h
        include/craft/docraft_craft_language_tokens.h
        src/utils/docraft_logger.cc
        include/utils/docraft_logger.h
)
add_library(docraft_lib STATIC
        ${DOCRAFT_SOURCES}
        ${GENERATED_BISON_SOURCES}
        ${GENERATED_FLEX_SOURCES}
)
generate_docraft_fonts(docraft_lib "${CMAKE_CURRENT_SOURCE_DIR}/fonts.json")
add_executable(docraft_exe src/main.cpp)
target_link_libraries(docraft_lib PUBLIC pugixml::static pugixml::pugixml ${LIBHARU_TARGET})
target_link_libraries(docraft_exe PRIVATE docraft_lib)
target_include_directories(docraft_lib PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

target_include_directories(docraft_lib PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
