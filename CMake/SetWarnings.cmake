function(setWarningFlagsForTarget target)
  if (DISABLE_STRICT_WARNINGS)
    return()
  endif()
  if (MSVC)
    target_compile_options(${target} PRIVATE /W3 /WX)
  else()
    target_compile_options(${target} PRIVATE -Wall -Wextra -pedantic -Werror -Wno-unused-parameter -fms-extensions -Wimplicit-fallthrough=5)
  endif()
endfunction()
