#gtest setup
find_package(GTest REQUIRED)
include(GoogleTest)
#source files
set(TEST_SOURCES
        docraft/main.cpp
        docraft/docraft_color_test.cc
        docraft/docraft_cursor_test.cc
        docraft/exception/docraft_exception_test.cc
        docraft/backend/docraft_haru_backend_test.cc
        docraft/utils/docraft_logger_test.cc
        docraft/utils/docraft_parser_utils.cc
        docraft/loom/pipeline/docraft_loom_measure_processor_test.cc
        docraft/loom/nodes/docraft_loom_stack_nodes_test.cc
        docraft/loom/nodes/docraft_loom_node_test.cc
        docraft/loom/nodes/docraft_loom_blank_line_test.cc
        docraft/loom/nodes/docraft_loom_image_test.cc
        docraft/loom/nodes/docraft_loom_line_test.cc
        docraft/loom/nodes/docraft_loom_rectangle_test.cc
        docraft/loom/nodes/docraft_loom_shape_nodes_test.cc
        docraft/loom/nodes/docraft_loom_list_test.cc
        docraft/loom/nodes/docraft_loom_table_test.cc
        docraft/backend/docraft_mock_backend.h
        docraft/craft/docraft_shape_parser_test.cc
        docraft/craft/docraft_image_parser_test.cc
        docraft/craft/docraft_table_parser_test.cc
        docraft/craft/docraft_craft_language_parser_test.cc
        docraft/craft/docraft_paragraph_parser_test.cc
        docraft/craft/docraft_section_parser_test.cc
        docraft/craft/docraft_loom_craft_language_parser_test.cc
        docraft/loom/craft/docraft_loom_tree_builder_test.cc
)
# NOTE: docraft/craft/docraft_settings_parser_test.cc and
# docraft/templating/docraft_template_engine_test.cc remain excluded -- Settings parsing
# (Phase 4) and loom templating (Phase 7) haven't been rewritten yet. The rest of
# docraft/craft/*_test.cc has been ported to target the generic DocraftParsedElement/
# std::any parser output (and, where a DocraftLoomTreeBuilder path exists, actual loom
# node output) instead of the deleted legacy model tree.

add_executable(docraft_test
        ${TEST_SOURCES}
)
target_link_libraries(docraft_test PRIVATE docraft GTest::GTest GTest::gmock)
target_include_directories(docraft_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
gtest_add_tests(TARGET docraft_test
        TEST_LIST DOCRAFT_TESTS
)
