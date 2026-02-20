


#gtest setup
find_package(GTest REQUIRED)
#source files
set(TEST_SOURCES
        main.cpp
        docraft_document_test.cc
        docraft_color_test.cc
        generic/docraft_font_applier_test.cc
        layout/docraft_cursor_test.cc
        layout/docraft_pagination_test.cc
        model/docraft_position_test.cc
        layout/docraft_layout_engine_test.cc
        layout/docraft_layout_text_handler_test.cc
        model/docraft_list_test.cc
        model/docraft_settings_test.cc
        model/docraft_node_test.cc
        model/docraft_z_index_test.cc
        craft/docraft_table_parser_test.cc
        craft/docraft_shape_parser_test.cc
        craft/docraft_image_parser_test.cc
        craft/docraft_settings_parser_test.cc
        backend/docraft_haru_backend_test.cc
        renderer/docraft_image_painter_test.cc
        renderer/docraft_painter_smoke_test.cc
        renderer/docraft_section_margin_test.cc
        templating/docraft_template_engine_test.cc)

add_executable(docraft_test
        ${TEST_SOURCES}
)
target_link_libraries(docraft_test PRIVATE docraft_lib GTest::GTest)
gtest_discover_tests(docraft_test
        DISCOVERY_MODE PRE_TEST
)
