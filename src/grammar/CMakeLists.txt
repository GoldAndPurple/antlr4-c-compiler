set(lexer_lib_name DlangLexer)
set(parser_lib_name DlangParser)

include(CompileOptions)

add_library(${lexer_lib_name} STATIC)
add_library(${parser_lib_name} STATIC)

find_package(ANTLR REQUIRED)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(antlr_lexer_target_name Antlr${lexer_lib_name})
set(antlr_parser_target_name Antlr${parser_lib_name})

antlr_target(
  ${antlr_lexer_target_name}
  ${lexer_lib_name}.g4
  LEXER
  PACKAGE "dlang"
)
target_set_cxx_standart(${lexer_lib_name})

target_sources(
  ${lexer_lib_name}
  PRIVATE
    ${ANTLR_${antlr_lexer_target_name}_CXX_OUTPUTS}
)

target_include_directories(
  ${lexer_lib_name}
  PUBLIC
    ${ANTLR_${antlr_lexer_target_name}_OUTPUT_DIR}
)

target_link_libraries(
  ${lexer_lib_name}
  PUBLIC
    antlr4
    Threads::Threads
)

antlr_target(
  ${antlr_parser_target_name}
  ${parser_lib_name}.g4
  DEPENDS_ANTLR ${antlr_lexer_target_name}
  PARSER
  VISITOR
  PACKAGE "dlang"
  COMPILE_FLAGS -lib "${ANTLR_${antlr_lexer_target_name}_OUTPUT_DIR}"
)

target_set_cxx_standart(${parser_lib_name})
target_sources(
  ${parser_lib_name}
  PRIVATE
    ${ANTLR_${antlr_parser_target_name}_CXX_OUTPUTS}
)
target_include_directories(
  ${parser_lib_name}
  PUBLIC
    ${ANTLR_${antlr_parser_target_name}_OUTPUT_DIR}
)
target_link_libraries(
  ${parser_lib_name}
  PUBLIC
    antlr4
    Threads::Threads
)
