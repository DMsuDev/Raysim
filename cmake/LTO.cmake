# == CMake module for enabling Link Time Optimization (LTO) =
include_guard()

function(rs_enable_lto target_name)
  if(NOT TARGET ${target_name})
    message(FATAL_ERROR "Target '${target_name}' does not exist")
  endif()

  if(NOT RS_ENABLE_LTO)
    return()
  endif()

  if(RS_ENABLE_SANITIZERS)
    message(STATUS "LTO disabled because sanitizers are enabled")
    return()
  endif()

  if(MINGW)
    message(STATUS "LTO is not supported on MinGW, skipping")
    return()
  endif()

  include(CheckIPOSupported)
  check_ipo_supported(RESULT supported OUTPUT error)

  if(supported)
    set_property(TARGET ${target_name} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
  else()
    message(WARNING "IPO/LTO not supported: ${error}")
  endif()
endfunction()
