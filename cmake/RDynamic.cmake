# == Cmake module for enabling -rdynamic on supported platforms ==
include_guard()

function(rs_enable_rdynamic target)
  if(NOT TARGET ${target})
    message(FATAL_ERROR "Target '${target}' does not exist")
  endif()

  if(WIN32)
    message(STATUS "Skipping -rdynamic for target '${target}' on Windows")
    return()
  endif()

  if(UNIX AND NOT APPLE)
    target_link_options(${target} PRIVATE -rdynamic)
  endif()
endfunction()
