set(app_name dlangparser)

add_executable(${app_name})

target_include_directories(${app_name} PRIVATE .)

include(CompileOptions)
set_compile_options(${app_name})

target_sources(
  ${app_name}
  PRIVATE
    ${app_name}/main.cpp
)

target_link_libraries(
  ${app_name}
  PRIVATE
    cxxopts
    dlang
)

set_target_properties(
  ${app_name}
  PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
