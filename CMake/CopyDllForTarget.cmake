function(copyDllForTarget target)
  if (WIN32)
    add_custom_command(TARGET ${target}
      POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different ${AWS_DLLS} $<TARGET_FILE:openvds> $<TARGET_FILE_DIR:${target}>)
  endif()
endfunction()