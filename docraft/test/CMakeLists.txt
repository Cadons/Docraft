


#gtest setup
find_package(GTest REQUIRED)
#source files
set(TEST_SOURCES
        main.cpp
        docraft_color_test.cc
        generic/docraft_font_applier_test.cc
        layout/docraft_cursor_test.cc
        model/docraft_position_test.cc
        layout/docraft_layout_engine_test.cc
        layout/docraft_layout_text_handler_test.cc
        model/docraft_list_test.cc
        renderer/docraft_image_painter_test.cc
        renderer/docraft_painter_smoke_test.cc)

add_executable(docraft_test
        ${TEST_SOURCES}
)
target_link_libraries(docraft_test PRIVATE docraft_lib GTest::GTest)
gtest_discover_tests(docraft_test
        DISCOVERY_MODE PRE_TEST
)
