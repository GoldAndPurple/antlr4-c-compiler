set(lib_name dlang)

add_library(${lib_name})

include(CompileOptions)
set_compile_options(${lib_name})

target_include_directories(${lib_name} PUBLIC .)

target_sources(
  ${lib_name}
  PUBLIC
    libdlang/dump_tokens.hpp
  PRIVATE
    libdlang/dump_tokens.cpp
)

target_link_libraries(
  ${lib_name}
  PRIVATE
    DlangLexer
    fmt
)

set(test_name libdlang_test)

add_executable(${test_name})

add_test(NAME ${test_name} COMMAND ${test_name})

set_compile_options(${test_name})

target_sources(
  ${test_name}
  PRIVATE
    test/lexer.cpp
)

target_link_libraries(
  ${test_name}
  PRIVATE
    ${lib_name}
    gtest
    gtest_main
)
