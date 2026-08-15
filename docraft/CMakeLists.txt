include(${CMAKE_CURRENT_LIST_DIR}/../.cmake/generate_fonts.cmake)

set(DOCRAFT_SOURCES
        src/docraft/docraft_position.cc
        include/docraft/docraft_position.h
        src/docraft/docraft_cursor.cc
        include/docraft/docraft_cursor.h
        src/docraft/docraft_color.cc
        include/docraft/docraft_color.h
        src/docraft/docraft_document_metadata.cc
        include/docraft/docraft_document_metadata.h
        include/docraft/backend/docraft_pdf_backend.h
        include/docraft/backend/docraft_text_rendering_backend.h
        include/docraft/backend/docraft_line_rendering_backend.h
        include/docraft/backend/docraft_shape_rendering_backend.h
        include/docraft/backend/docraft_image_rendering_backend.h
        include/docraft/backend/docraft_page_rendering_backend.h
        include/docraft/backend/docraft_page_format.h
        include/docraft/backend/docraft_backend_providers_factory.h
        include/docraft/backend/docraft_rendering_backend.h
        include/docraft/backend/docraft_output_backend.h
        include/docraft/backend/docraft_font_backend.h
        include/docraft/backend/docraft_metadata_backend.h
        include/docraft/utils/docraft_font_registry.h
        src/docraft/utils/docraft_logger.cc
        src/docraft/utils/docraft_base64.cc
        src/docraft/utils/docraft_font_resolver.cc
        src/docraft/utils/docraft_font_registry.cc
        src/docraft/utils/docraft_parser_utilis.cc
        src/docraft/utils/docraft_utf8.cc
        src/docraft/utils/docraft_file_utils.cc
        include/docraft/utils/docraft_logger.h
        include/docraft/utils/docraft_base64.h
        include/docraft/utils/docraft_font_resolver.h
        include/docraft/utils/docraft_parser_utilis.h
        include/docraft/utils/docraft_utf8.h
        include/docraft/utils/docraft_file_utils.h
        src/docraft/backend/pdf/docraft_haru_backend.cc
        include/docraft/backend/pdf/docraft_haru_backend.h
        src/docraft/backend/pdf/docraft_haru_backend_providers_factory.cc
        include/docraft/backend/pdf/docraft_haru_backend_providers_factory.h
        src/docraft/backend/pdf/docraft_haru_output_backend.cc
        include/docraft/backend/pdf/docraft_haru_output_backend.h
        src/docraft/backend/pdf/docraft_haru_font_backend.cc
        include/docraft/backend/pdf/docraft_haru_font_backend.h
        src/docraft/backend/pdf/docraft_haru_metadata_backend.cc
        include/docraft/backend/pdf/docraft_haru_metadata_backend.h
        src/docraft/backend/pdf/docraft_haru_text_backend.cc
        include/docraft/backend/pdf/docraft_haru_text_backend.h
        src/docraft/backend/pdf/docraft_haru_line_backend.cc
        include/docraft/backend/pdf/docraft_haru_line_backend.h
        src/docraft/backend/pdf/docraft_haru_shape_backend.cc
        include/docraft/backend/pdf/docraft_haru_shape_backend.h
        src/docraft/backend/pdf/docraft_haru_image_backend.cc
        include/docraft/backend/pdf/docraft_haru_image_backend.h
        src/docraft/backend/pdf/docraft_haru_page_backend.cc
        include/docraft/backend/pdf/docraft_haru_page_backend.h
        include/docraft/backend/pdf/docraft_haru_shared_state.h
        src/docraft/templating/docraft_template_engine.cc
        include/docraft/templating/docraft_template_engine.h
        include/docraft/exception/docraft_exceptions.h
        src/docraft/loom/nodes/docraft_loom_text.cc
        include/docraft/loom/nodes/docraft_loom_text.h
        src/docraft/loom/nodes/docraft_loom_title.cc
        include/docraft/loom/nodes/docraft_loom_title.h
        src/docraft/loom/nodes/docraft_loom_subtitle.cc
        include/docraft/loom/nodes/docraft_loom_subtitle.h
        src/docraft/loom/nodes/docraft_loom_rectangle.cc
        include/docraft/loom/nodes/docraft_loom_rectangle.h
        src/docraft/loom/nodes/docraft_loom_canvas.cc
        include/docraft/loom/nodes/docraft_loom_canvas.h
        include/docraft/loom/nodes/docraft_loom_shape_style.h
        src/docraft/loom/nodes/docraft_loom_shape.cc
        include/docraft/loom/nodes/docraft_loom_shape.h
        src/docraft/loom/nodes/docraft_loom_layout_container.cc
        include/docraft/loom/nodes/docraft_loom_layout_container.h
        src/docraft/loom/nodes/docraft_loom_circle.cc
        include/docraft/loom/nodes/docraft_loom_circle.h
        src/docraft/loom/nodes/docraft_loom_triangle.cc
        include/docraft/loom/nodes/docraft_loom_triangle.h
        src/docraft/loom/nodes/docraft_loom_polygon.cc
        include/docraft/loom/nodes/docraft_loom_polygon.h
        src/docraft/loom/nodes/docraft_loom_curve_line.cc
        include/docraft/loom/nodes/docraft_loom_curve_line.h
        src/docraft/loom/nodes/docraft_loom_list.cc
        include/docraft/loom/nodes/docraft_loom_list.h
        src/docraft/loom/nodes/docraft_loom_table_cell.cc
        include/docraft/loom/nodes/docraft_loom_table_cell.h
        src/docraft/loom/nodes/docraft_loom_table.cc
        include/docraft/loom/nodes/docraft_loom_table.h
        src/docraft/loom/nodes/docraft_loom_page_number.cc
        include/docraft/loom/nodes/docraft_loom_page_number.h
        include/docraft/loom/interfaces/docraft_loom_visitor.h
        include/docraft/loom/nodes/docraft_loom_text.h
        src/docraft/loom/nodes/docraft_loom_node.cc
        include/docraft/loom/nodes/docraft_loom_node.h
        src/docraft/loom/pipeline/docraft_loom_measure_processor.cc
        include/docraft/loom/pipeline/docraft_loom_measure_processor.h
        src/docraft/loom/pipeline/docraft_loom_weighted_distribution.cc
        include/docraft/loom/pipeline/docraft_loom_weighted_distribution.h
        src/docraft/loom/nodes/docraft_loom_paragraph.cc
        include/docraft/loom/nodes/docraft_loom_paragraph.h
        src/docraft/loom/nodes/docraft_loom_vstack.cc
        include/docraft/loom/nodes/docraft_loom_vstack.h
        src/docraft/loom/nodes/docraft_loom_hstack.cc
        include/docraft/loom/nodes/docraft_loom_hstack.h
        src/docraft/loom/pipeline/docraft_loom_layout_processor.cc
        include/docraft/loom/pipeline/docraft_loom_layout_processor.h
        src/docraft/loom/pipeline/docraft_loom_pagination_processor.cc
        include/docraft/loom/pipeline/docraft_loom_pagination_processor.h
        src/docraft/loom/pipeline/docraft_loom_shape_draw_utils.cc
        include/docraft/loom/pipeline/docraft_loom_shape_draw_utils.h
        src/docraft/loom/pipeline/docraft_loom_rendering_processor.cc
        include/docraft/loom/pipeline/docraft_loom_rendering_processor.h
        src/docraft/loom/pipeline/docraft_loom_pipeline_executor.cc
        include/docraft/loom/pipeline/docraft_loom_pipeline_executor.h
        src/docraft/loom/docraft_loom_pdf_creator.cc
        include/docraft/loom/docraft_loom_pdf_creator.h
        src/docraft/loom/nodes/docraft_loom_blank_line.cc
        include/docraft/loom/nodes/docraft_loom_blank_line.h
        src/docraft/loom/nodes/docraft_loom_image.cc
        include/docraft/loom/nodes/docraft_loom_image.h
        src/docraft/loom/nodes/docraft_loom_line.cc
        include/docraft/loom/nodes/docraft_loom_line.h
        src/docraft/loom/nodes/docraft_loom_new_page.cc
        include/docraft/loom/nodes/docraft_loom_new_page.h
        include/docraft/loom/docraft_loom_foreach.h

        # Craft-language parser: generic, zero dependency on docraft/loom (Phase 2).
        include/docraft/craft/docraft_craft_parsed_element.h
        include/docraft/craft/docraft_craft_language_tokens.h
        include/docraft/craft/i_docraft_parser.h
        src/docraft/craft/docraft_craft_language_parser.cc
        include/docraft/craft/docraft_craft_language_parser.h
        src/docraft/craft/parser/docraft_parser_helpers.cc
        include/docraft/craft/parser/docraft_parser_helpers.h
        src/docraft/craft/parser/docraft_shape_parser_utils.cc
        include/docraft/craft/parser/docraft_shape_parser_utils.h
        include/docraft/craft/parser/docraft_parser.h
        src/docraft/craft/parser/docraft_rectangle_parser.cc
        src/docraft/craft/parser/docraft_chart_parser.cc
        src/docraft/craft/parser/docraft_series_parser.cc
        src/docraft/craft/parser/docraft_text_parser.cc
        src/docraft/craft/parser/docraft_page_number_parser.cc
        src/docraft/craft/parser/docraft_image_parser.cc
        src/docraft/craft/parser/docraft_blank_line_parser.cc
        src/docraft/craft/parser/docraft_list_parser.cc
        src/docraft/craft/parser/docraft_layout_parser.cc
        src/docraft/craft/parser/docraft_table_parser.cc
        include/docraft/craft/parser/docraft_circle_parser.h
        src/docraft/craft/parser/docraft_circle_parser.cc
        include/docraft/craft/parser/docraft_triangle_parser.h
        src/docraft/craft/parser/docraft_triangle_parser.cc
        include/docraft/craft/parser/docraft_polygon_parser.h
        src/docraft/craft/parser/docraft_polygon_parser.cc
        include/docraft/craft/parser/docraft_curve_line_parser.h
        src/docraft/craft/parser/docraft_curve_line_parser.cc
        include/docraft/craft/parser/docraft_line_parser.h
        src/docraft/craft/parser/docraft_line_parser.cc
        include/docraft/craft/parser/docraft_paragraph_parser.h
        src/docraft/craft/parser/docraft_paragraph_parser.cc
        include/docraft/craft/parser/docraft_section_parsers.h
        src/docraft/craft/parser/docraft_section_parsers.cc
        src/docraft/craft/parser/docraft_new_page_parser.cc
        include/docraft/craft/parser/docraft_foreach_parser.h
        src/docraft/craft/parser/docraft_foreach_parser.cc

        # Loom-side tree builder: the only component depending on both docraft/craft and
        # docraft/loom (translates DocraftParsedElement trees into loom nodes).
        include/docraft/loom/craft/docraft_loom_tree_builder.h
        src/docraft/loom/craft/docraft_loom_tree_builder.cc

        # docraft/loom/charts (Chart/Series craft tags expand into a populated Canvas via
        # a style registry -- Docraft's "chart plugin" mechanism; see DocraftChartBuilderRegistry).
        include/docraft/loom/charts/docraft_chart_types.h
        src/docraft/loom/charts/docraft_chart_types.cc
        include/docraft/loom/charts/docraft_chart_axis_helpers.h
        src/docraft/loom/charts/docraft_chart_axis_helpers.cc
        include/docraft/loom/charts/docraft_chart_builder.h
        src/docraft/loom/charts/docraft_chart_builder.cc
        include/docraft/loom/charts/docraft_scatter_chart_builder.h
        src/docraft/loom/charts/docraft_scatter_chart_builder.cc
        include/docraft/loom/charts/docraft_spline_chart_builder.h
        src/docraft/loom/charts/docraft_spline_chart_builder.cc
        include/docraft/loom/charts/docraft_line_chart_builder.h
        src/docraft/loom/charts/docraft_line_chart_builder.cc
        include/docraft/loom/charts/docraft_histogram_chart_builder.h
        src/docraft/loom/charts/docraft_histogram_chart_builder.cc
        include/docraft/loom/charts/docraft_pie_chart_builder.h
        src/docraft/loom/charts/docraft_pie_chart_builder.cc
        src/docraft/loom/charts/docraft_builtin_charts.cc

        # Top-level driver (Phase 3): walks a full <Document> and builds a fully-wired
        # DocraftLoomPdfCreator from its Header/Body/Footer children.
        include/docraft/craft/docraft_loom_craft_language_parser.h
        src/docraft/craft/docraft_loom_craft_language_parser.cc
)
set(DOCRAFT_LIBRARY_TYPE STATIC)
if (BUILD_SHARED_LIBS)
    set(DOCRAFT_LIBRARY_TYPE SHARED)
endif ()
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

if (BUILD_SHARED_LIBS)
    target_compile_definitions(docraft PRIVATE DOCRAFT_BUILD_SHARED_LIBS)#define the macro to build the shared lib
    target_compile_definitions(docraft INTERFACE DOCRAFT_USE_SHARED_LIB)#define the macro to use the shared lib
endif ()

generate_docraft_fonts(docraft "${CMAKE_CURRENT_SOURCE_DIR}/fonts.json")

add_executable(docraft_tool src/docraft/main.cpp)
#declare define in the c++ code to export version
target_compile_definitions(docraft_tool PRIVATE DOCRAFT_VERSION="${DOCRAFT_LIBRARY_VERSION}")
configure_artifact_version(docraft_tool ${PROJECT_VERSION})

if (TARGET pugixml::static)
    set(PUGIXML_TARGET pugixml::static)
elseif (TARGET pugixml::pugixml)
    set(PUGIXML_TARGET pugixml::pugixml)
else ()
    message(FATAL_ERROR "No pugixml CMake target found")
endif ()

target_link_libraries(docraft PUBLIC ${PUGIXML_TARGET} nlohmann_json::nlohmann_json ${LIBHARU_TARGET} fmt::fmt)
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
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
