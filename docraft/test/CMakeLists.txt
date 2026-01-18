


#gtest setup
find_package(GTest REQUIRED)
#source files
set(TEST_SOURCES
        main.cpp
        test.cpp
        docraft_color_test.cc
        layout/docraft_cursor_test.cc
        model/docraft_position_test.cc)

add_executable(docraft_test
        ${TEST_SOURCES}
)
target_link_libraries(docraft_test PRIVATE docraft_lib GTest::GTest)
gtest_discover_tests(docraft_test
        DISCOVERY_MODE PRE_TEST
)