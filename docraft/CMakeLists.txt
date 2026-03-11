include(${CMAKE_CURRENT_LIST_DIR}/../.cmake/generate_fonts.cmake)

set(DOCRAFT_SOURCES
        src/docraft/model/docraft_node.cc
        include/docraft/model/docraft_node.h
        src/docraft/model/docraft_clone_utils.cc
        include/docraft/model/docraft_clone_utils.h
        include/docraft/model/i_docraft_clonable.h
        src/docraft/model/docraft_section.cc
        include/docraft/model/docraft_section.h
        src/docraft/model/docraft_body.cc
        include/docraft/model/docraft_body.h
        src/docraft/model/docraft_text.cc
        include/docraft/model/docraft_text.h
        src/docraft/model/docraft_page_number.cc
        include/docraft/model/docraft_page_number.h
        src/docraft/model/docraft_new_page.cc
        include/docraft/model/docraft_new_page.h
        src/docraft/docraft_document.cc
        include/docraft/docraft_document.h
        src/docraft/docraft_document_context.cc
        include/docraft/docraft_document_context.h
        src/docraft/docraft_cursor.cc
        include/docraft/docraft_cursor.h
        src/docraft/docraft_color.cc
        include/docraft/docraft_color.h
        include/docraft/renderer/docraft_renderer.h
        src/docraft/renderer/docraft_pdf_renderer.cc
        include/docraft/renderer/docraft_pdf_renderer.h
        src/docraft/renderer/painter/docraft_text_painter.cc
        include/docraft/renderer/painter/docraft_text_painter.h
        include/docraft/renderer/painter/i_painter.h
        src/docraft/layout/docraft_layout_engine.cc
        include/docraft/layout/docraft_layout_engine.h
        src/docraft/layout/handler/docraft_layout_text_handler.cc
        include/docraft/layout/handler/docraft_layout_text_handler.h
        include/docraft/generic/chain_of_responsibility_handler.h
        include/docraft/layout/handler/abstract_docraft_layout_handler.h
        src/docraft/generic/docraft_font_applier.cc
        include/docraft/generic/docraft_font_applier.h
        src/docraft/model/docraft_header.cc
        include/docraft/model/docraft_header.h
        src/docraft/model/docraft_paragraph.cc
        include/docraft/model/docraft_paragraph.h
        src/docraft/model/docraft_layout.cc
        include/docraft/model/docraft_layout.h
        src/docraft/model/docraft_list.cc
        include/docraft/model/docraft_list.h
        src/docraft/layout/handler/docraft_layout_handler.cc
        include/docraft/layout/handler/docraft_layout_handler.h
        src/docraft/layout/handler/docraft_layout_list_handler.cc
        include/docraft/layout/handler/docraft_layout_list_handler.h
        src/docraft/model/docraft_children_container_node.cc
        include/docraft/model/docraft_children_container_node.h
        include/docraft/backend/docraft_pdf_backend.h
        include/docraft/backend/docraft_text_rendering_backend.h
        include/docraft/backend/docraft_line_rendering_backend.h
        include/docraft/backend/docraft_shape_rendering_backend.h
        include/docraft/backend/docraft_image_rendering_backend.h
        include/docraft/backend/docraft_page_rendering_backend.h
        include/docraft/backend/docraft_rendering_backend.h
        src/docraft/model/docraft_image.cc
        include/docraft/model/docraft_image.h
        src/docraft/renderer/painter/docraft_image_painter.cc
        include/docraft/renderer/painter/docraft_image_painter.h
        src/docraft/layout/handler/docraft_basic_layout_handler.cc
        include/docraft/layout/handler/docraft_basic_layout_handler.h
        src/docraft/model/docraft_footer.cc
        include/docraft/model/docraft_footer.h
        src/docraft/model/docraft_table.cc
        include/docraft/model/docraft_table.h
        src/docraft/renderer/painter/docraft_table_painter.cc
        include/docraft/renderer/painter/docraft_table_painter.h
        src/docraft/layout/handler/docraft_layout_table_handler.cc
        include/docraft/layout/handler/docraft_layout_table_handler.h
        include/docraft/utils/docraft_font_registry.h
        src/docraft/model/docraft_rectangle.cc
        include/docraft/model/docraft_rectangle.h
        src/docraft/model/docraft_circle.cc
        include/docraft/model/docraft_circle.h
        src/docraft/model/docraft_triangle.cc
        include/docraft/model/docraft_triangle.h
        src/docraft/model/docraft_line.cc
        include/docraft/model/docraft_line.h
        src/docraft/model/docraft_polygon.cc
        include/docraft/model/docraft_polygon.h
        src/docraft/renderer/painter/docraft_rectangle_painter.cc
        include/docraft/renderer/painter/docraft_rectangle_painter.h
        src/docraft/renderer/painter/docraft_circle_painter.cc
        include/docraft/renderer/painter/docraft_circle_painter.h
        src/docraft/renderer/painter/docraft_triangle_painter.cc
        include/docraft/renderer/painter/docraft_triangle_painter.h
        src/docraft/renderer/painter/docraft_line_painter.cc
        include/docraft/renderer/painter/docraft_line_painter.h
        src/docraft/renderer/painter/docraft_polygon_painter.cc
        include/docraft/renderer/painter/docraft_polygon_painter.h
        src/docraft/model/docraft_blank_line.cc
        include/docraft/model/docraft_blank_line.h
        src/docraft/layout/handler/docraft_layout_blank_line.cc
        include/docraft/layout/handler/docraft_layout_blank_line.h
        src/docraft/renderer/painter/docraft_blank_line_painter.cc
        include/docraft/renderer/painter/docraft_blank_line_painter.h
        src/docraft/craft/docraft_craft_language_parser.cc
        include/docraft/craft/docraft_craft_language_parser.h
        include/docraft/craft/i_docraft_parser.h
        src/docraft/craft/parser/docraft_parser_helpers.cc
        src/docraft/craft/parser/docraft_rectangle_parser.cc
        src/docraft/craft/parser/docraft_section_parsers.cc
        src/docraft/craft/parser/docraft_text_parser.cc
        src/docraft/craft/parser/docraft_page_number_parser.cc
        src/docraft/craft/parser/docraft_image_parser.cc
        src/docraft/craft/parser/docraft_table_parser.cc
        src/docraft/craft/parser/docraft_layout_parser.cc
        src/docraft/craft/parser/docraft_list_parser.cc
        src/docraft/craft/parser/docraft_blank_line_parser.cc
        src/docraft/craft/parser/docraft_new_page_parser.cc
        src/docraft/craft/parser/docraft_circle_parser.cc
        src/docraft/craft/parser/docraft_triangle_parser.cc
        src/docraft/craft/parser/docraft_line_parser.cc
        src/docraft/craft/parser/docraft_polygon_parser.cc
        src/docraft/craft/parser/docraft_shape_parser_utils.cc
        src/docraft/craft/parser/docraft_settings_parser.cc
        include/docraft/craft/parser/docraft_parser.h
        include/docraft/craft/parser/docraft_circle_parser.h
        include/docraft/craft/parser/docraft_triangle_parser.h
        include/docraft/craft/parser/docraft_line_parser.h
        include/docraft/craft/parser/docraft_polygon_parser.h
        include/docraft/craft/parser/docraft_shape_parser_utils.h
        include/docraft/craft/docraft_craft_language_tokens.h
        src/docraft/utils/docraft_logger.cc
        src/docraft/utils/docraft_base64.cc
        src/docraft/utils/docraft_font_resolver.cc
        src/docraft/utils/docraft_keyword_extractor.cc
        src/docraft/utils/docraft_font_registry.cc
        src/docraft/utils/docraft_parser_utilis.cc
        include/docraft/utils/docraft_logger.h
        include/docraft/utils/docraft_base64.h
        include/docraft/utils/docraft_keyword_extractor.h
        include/docraft/utils/docraft_parser_utilis.h
        include/docraft/model/docraft_position.h
        src/docraft/model/docraft_position.cc
        src/docraft/backend/pdf/docraft_haru_backend.cc
        include/docraft/backend/pdf/docraft_haru_backend.h
        src/docraft/model/docraft_settings.cc
        include/docraft/model/docraft_settings.h
        include/docraft/model/docraft_page_format.h
        src/docraft/templating/docraft_template_engine.cc
        include/docraft/templating/docraft_template_engine.h
        src/docraft/model/docraft_foreach.cc
        include/docraft/model/docraft_foreach.h
        src/docraft/craft/parser/docraft_foreach_parser.cc
)
set(DOCRAFT_LIBRARY_TYPE STATIC)
if(BUILD_SHARED_LIB)
    set(DOCRAFT_LIBRARY_TYPE SHARED)
endif()
add_library(docraft ${DOCRAFT_LIBRARY_TYPE}
        ${DOCRAFT_SOURCES}
)
add_library(docraft::docraft ALIAS docraft)
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
add_executable(docraft_tool src/docraft/main.cpp)
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

target_include_directories(docraft
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>#include dir for the build tree (for generated headers)
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>#include dir for the build tree
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>#include dir for the install tree
)

#install rules
install(TARGETS docraft
        EXPORT docraftTargets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(TARGETS docraft_tool
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
install(
        EXPORT docraftTargets
        FILE docraftTargets.cmake
        NAMESPACE docraft::
        DESTINATION "${DOCRAFT_INSTALL_CMAKEDIR}"
)
install(DIRECTORY include/docraft DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/docraft)
