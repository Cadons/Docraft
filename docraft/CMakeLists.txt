set(DOCRAFT_SOURCES src/main.cpp)
configure_lexer_and_bison(
        ${CMAKE_CURRENT_LIST_DIR}/compiler/grammar.y
        ${CMAKE_CURRENT_LIST_DIR}/compiler/parser.l
        GENERATED_BISON_SOURCES
        GENERATED_FLEX_SOURCES)
add_library(docraft_lib STATIC
        ${DOCRAFT_SOURCES}
        ${GENERATED_BISON_SOURCES}
        ${GENERATED_FLEX_SOURCES}
)
target_include_directories(docraft_lib PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
