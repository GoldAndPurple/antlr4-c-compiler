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