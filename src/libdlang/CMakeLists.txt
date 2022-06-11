set(lib_name dlang)

add_library(${lib_name})

include(CompileOptions)
set_compile_options(${lib_name})

target_include_directories(${lib_name} PUBLIC .)

# LLVM libs setup

find_package(LLVM REQUIRED CONFIG)
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

llvm_map_components_to_libnames(llvm_libs core support irreader)

target_sources(
  ${lib_name}
  PUBLIC
    libdlang/ast.hpp
    libdlang/options.hpp
    libdlang/codegen.hpp
  PRIVATE
    libdlang/ast.cpp
    libdlang/options.cpp
)

target_link_libraries(
  ${lib_name}
  PRIVATE
    DlangLexer
    DlangParser
    ${llvm_libs}
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
