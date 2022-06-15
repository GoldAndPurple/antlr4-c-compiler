cmake_minimum_required(VERSION 3.13)

project(
  DlangParser
  VERSION 1.0
  LANGUAGES CXX
)

list(
  APPEND
  CMAKE_MODULE_PATH
  ${CMAKE_CURRENT_LIST_DIR}/cmake
  ${CMAKE_CURRENT_LIST_DIR}/cmake/antlr4
)

add_subdirectory(src)
add_subdirectory(thirdparty)
