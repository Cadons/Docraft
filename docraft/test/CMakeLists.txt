


#gtest setup
find_package(GTest REQUIRED)
#source files
set(TEST_SOURCES
        main.cpp
        test.cpp
        docraft_color_text.cc
        layout/DocraftCursorTest.cc)

add_executable(docraft_test
        ${TEST_SOURCES}
)
target_link_libraries(docraft_test PRIVATE docraft_lib GTest::GTest)
gtest_discover_tests(docraft_test
        DISCOVERY_MODE PRE_TEST
)