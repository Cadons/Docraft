#gtest setup
find_package(GTest REQUIRED)
include(GoogleTest)
#source files
set(TEST_SOURCES
        docraft/main.cpp
        docraft/docraft_document_test.cc
        docraft/docraft_color_test.cc
        docraft/generic/docraft_font_applier_test.cc
        docraft/layout/docraft_cursor_test.cc
        docraft/layout/docraft_pagination_test.cc
        docraft/model/docraft_position_test.cc
        docraft/layout/docraft_layout_engine_test.cc
        docraft/layout/docraft_layout_text_handler_test.cc
        docraft/model/docraft_list_test.cc
        docraft/model/docraft_clone_test.cc
        docraft/model/docraft_settings_test.cc
        docraft/model/docraft_node_test.cc
        docraft/model/docraft_z_index_test.cc
        docraft/exception/docraft_exception_test.cc
        docraft/craft/docraft_table_parser_test.cc
        docraft/craft/docraft_craft_language_parser_test.cc
        docraft/craft/docraft_shape_parser_test.cc
        docraft/craft/docraft_image_parser_test.cc
        docraft/craft/docraft_settings_parser_test.cc
        docraft/backend/docraft_haru_backend_test.cc
        docraft/renderer/docraft_image_painter_test.cc
        docraft/renderer/docraft_painter_smoke_test.cc
        docraft/renderer/docraft_section_margin_test.cc
        docraft/templating/docraft_template_engine_test.cc
        docraft/utils/docraft_keyword_extractor_test.cc
        docraft/utils/docraft_logger_test.cc
        docraft/utils/docraft_parser_utils.cc
        docraft/loom/pipeline/docraft_loom_measure_processor_test.cpp
        docraft/backend/docraft_mock_backend.h
)

set(STRESS_TEST_SOURCES
        docraft/main.cpp
        docraft/layout/docraft_pagination_stress_test.cc
)

add_executable(docraft_test
        ${TEST_SOURCES}
)
target_link_libraries(docraft_test PRIVATE docraft GTest::GTest GTest::gmock)
target_include_directories(docraft_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
gtest_add_tests(TARGET docraft_test
        TEST_LIST DOCRAFT_TESTS
)

add_executable(docraft_stress_test
        ${STRESS_TEST_SOURCES}
)
target_link_libraries(docraft_stress_test PRIVATE docraft GTest::GTest GTest::gmock)
target_include_directories(docraft_stress_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
gtest_add_tests(TARGET docraft_stress_test
        TEST_PREFIX "Stress."
        TEST_LIST DOCRAFT_STRESS_TESTS
)
if (DOCRAFT_STRESS_TESTS)
    set_tests_properties(${DOCRAFT_STRESS_TESTS} PROPERTIES LABELS "stress")
endif ()
