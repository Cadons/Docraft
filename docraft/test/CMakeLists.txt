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
)
# NOTE: docraft/craft/*_test.cc and docraft/templating/docraft_template_engine_test.cc are
# temporarily excluded while the craft parser (Phase 2/3) and loom templating (Phase 7) are
# being rewritten -- re-add as each is rewritten to target loom instead of the deleted
# legacy model tree.

add_executable(docraft_test
        ${TEST_SOURCES}
)
target_link_libraries(docraft_test PRIVATE docraft GTest::GTest GTest::gmock)
target_include_directories(docraft_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
gtest_add_tests(TARGET docraft_test
        TEST_LIST DOCRAFT_TESTS
)
