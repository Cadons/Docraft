include(${CMAKE_CURRENT_LIST_DIR}/../.cmake/generate_fonts.cmake)

set(DOCRAFT_SOURCES
        src/model/docraft_node.cc
        include/model/docraft_node.h
        src/model/docraft_clone_utils.cc
        include/model/docraft_clone_utils.h
        include/model/i_docraft_clonable.h
        src/model/docraft_section.cc
        include/model/docraft_section.h
        src/model/docraft_body.cc
        include/model/docraft_body.h
        src/model/docraft_text.cc
        include/model/docraft_text.h
        src/model/docraft_page_number.cc
        include/model/docraft_page_number.h
        src/model/docraft_new_page.cc
        include/model/docraft_new_page.h
        src/docraft_document.cc
        include/docraft_document.h
        src/docraft_document_context.cc
        include/docraft_document_context.h
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
        src/generic/docraft_font_applier.cc
        include/generic/docraft_font_applier.h
        src/model/docraft_header.cc
        include/model/docraft_header.h
        src/model/docraft_paragraph.cc
        include/model/docraft_paragraph.h
        src/model/docraft_layout.cc
        include/model/docraft_layout.h
        src/model/docraft_list.cc
        include/model/docraft_list.h
        src/layout/handler/docraft_layout_handler.cc
        include/layout/handler/docraft_layout_handler.h
        src/layout/handler/docraft_layout_list_handler.cc
        include/layout/handler/docraft_layout_list_handler.h
        src/model/docraft_children_container_node.cc
        include/model/docraft_children_container_node.h
        include/backend/docraft_pdf_backend.h
        include/backend/docraft_text_rendering_backend.h
        include/backend/docraft_line_rendering_backend.h
        include/backend/docraft_shape_rendering_backend.h
        include/backend/docraft_image_rendering_backend.h
        include/backend/docraft_page_rendering_backend.h
        include/backend/docraft_rendering_backend.h
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
        src/model/docraft_circle.cc
        include/model/docraft_circle.h
        src/model/docraft_triangle.cc
        include/model/docraft_triangle.h
        src/model/docraft_line.cc
        include/model/docraft_line.h
        src/model/docraft_polygon.cc
        include/model/docraft_polygon.h
        src/renderer/painter/docraft_rectangle_painter.cc
        include/renderer/painter/docraft_rectangle_painter.h
        src/renderer/painter/docraft_circle_painter.cc
        include/renderer/painter/docraft_circle_painter.h
        src/renderer/painter/docraft_triangle_painter.cc
        include/renderer/painter/docraft_triangle_painter.h
        src/renderer/painter/docraft_line_painter.cc
        include/renderer/painter/docraft_line_painter.h
        src/renderer/painter/docraft_polygon_painter.cc
        include/renderer/painter/docraft_polygon_painter.h
        src/model/docraft_blank_line.cc
        include/model/docraft_blank_line.h
        src/layout/handler/docraft_layout_blank_line.cc
        include/layout/handler/docraft_layout_blank_line.h
        src/renderer/painter/docraft_blank_line_painter.cc
        include/renderer/painter/docraft_blank_line_painter.h
        src/craft/docraft_craft_language_parser.cc
        include/craft/docraft_craft_language_parser.h
        include/craft/i_docraft_parser.h
        src/craft/parser/docraft_parser_helpers.cc
        src/craft/parser/docraft_rectangle_parser.cc
        src/craft/parser/docraft_section_parsers.cc
        src/craft/parser/docraft_text_parser.cc
        src/craft/parser/docraft_page_number_parser.cc
        src/craft/parser/docraft_image_parser.cc
        src/craft/parser/docraft_table_parser.cc
        src/craft/parser/docraft_layout_parser.cc
        src/craft/parser/docraft_list_parser.cc
        src/craft/parser/docraft_blank_line_parser.cc
        src/craft/parser/docraft_new_page_parser.cc
        src/craft/parser/docraft_circle_parser.cc
        src/craft/parser/docraft_triangle_parser.cc
        src/craft/parser/docraft_line_parser.cc
        src/craft/parser/docraft_polygon_parser.cc
        src/craft/parser/docraft_shape_parser_utils.cc
        src/craft/parser/docraft_settings_parser.cc
        include/craft/parser/docraft_parser.h
        include/craft/parser/docraft_circle_parser.h
        include/craft/parser/docraft_triangle_parser.h
        include/craft/parser/docraft_line_parser.h
        include/craft/parser/docraft_polygon_parser.h
        include/craft/parser/docraft_shape_parser_utils.h
        include/craft/docraft_craft_language_tokens.h
        src/utils/docraft_logger.cc
        src/utils/docraft_base64.cc
        src/utils/docraft_font_resolver.cc
        src/utils/docraft_keyword_extractor.cc
        src/utils/docraft_font_registry.cc
        src/utils/docraft_parser_utilis.cc
        include/utils/docraft_logger.h
        include/utils/docraft_base64.h
        include/utils/docraft_keyword_extractor.h
        include/utils/docraft_parser_utilis.h
        include/model/docraft_position.h
        src/model/docraft_position.cc
        src/backend/pdf/docraft_haru_backend.cc
        include/backend/pdf/docraft_haru_backend.h
        src/model/docraft_settings.cc
        include/model/docraft_settings.h
        include/model/docraft_page_format.h
        src/templating/docraft_template_engine.cc
        include/templating/docraft_template_engine.h
        src/model/docraft_foreach.cc
        include/model/docraft_foreach.h
        src/craft/parser/docraft_foreach_parser.cc
)
set(DOCRAFT_LIBRARY_TYPE STATIC)
if(BUILD_SHARED_LIB)
    set(DOCRAFT_LIBRARY_TYPE SHARED)
endif()
add_library(docraft ${DOCRAFT_LIBRARY_TYPE}
        ${DOCRAFT_SOURCES}
)
configure_artifact_version(docraft ${PROJECT_VERSION})
configure_artifact_soversion(docraft ${PROJECT_VERSION_MAJOR})

# Export debug compile flags to dependents (tool/tests) only for Debug config.
target_compile_definitions(docraft PUBLIC
        $<$<CONFIG:Debug>:DOCRAFT_DEBUG>
        $<$<CONFIG:Debug>:docraft_debugf>
)

if(BUILD_SHARED_LIB)
    target_compile_definitions(docraft PRIVATE DOCRAFT_BUILD_SHARED_LIB)#define the macro to build the shared lib
    target_compile_definitions(docraft INTERFACE DOCRAFT_USE_SHARED_LIB)#define the macro to use the shared lib
endif()

generate_docraft_fonts(docraft "${CMAKE_CURRENT_SOURCE_DIR}/fonts.json")
add_executable(docraft_tool src/main.cpp)
#declare define in the c++ code to export version
target_compile_definitions(docraft_tool PRIVATE DOCRAFT_VERSION="${DOCRAFT_LIBRARY_VERSION}")
configure_artifact_version(docraft_tool ${PROJECT_VERSION})
if(TARGET pugixml::static)
    set(PUGIXML_TARGET pugixml::static)
elseif(TARGET pugixml::pugixml)
    set(PUGIXML_TARGET pugixml::pugixml)
else()
    message(FATAL_ERROR "No pugixml CMake target found")
endif()

target_link_libraries(docraft PUBLIC ${PUGIXML_TARGET} nlohmann_json::nlohmann_json ${LIBHARU_TARGET})
target_link_libraries(docraft_tool PRIVATE docraft)
target_include_directories(docraft PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

target_include_directories(docraft PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)

#install rules
install(TARGETS docraft
        EXPORT docraftTargets
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
)
install(TARGETS docraft_tool
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin
)
install(DIRECTORY include/ DESTINATION include)