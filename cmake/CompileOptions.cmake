function(target_set_cxx_standart target_name)
  set_target_properties(
    ${target_name}
    PROPERTIES
      CXX_STANDARD 17
      CXX_STANDARD_REQUIRED ON
      CXX_EXTENSIONS OFF
  )
endfunction()

function(set_compile_options target_name)
  if(MSVC)
    target_compile_options(${target_name} PRIVATE /W4 /WX)
  else()
    target_compile_options(${target_name} PRIVATE -Wno-unused-parameter -Wall -Wextra -Werror -pedantic)
  endif()

  target_set_cxx_standart(${target_name})

  if(CLANG_TIDY_EXE)
    set_target_properties(
      ${target_name}
      PROPERTIES
        CXX_CLANG_TIDY ${CLANG_TIDY_EXE}
    )
  endif()
endfunction()
