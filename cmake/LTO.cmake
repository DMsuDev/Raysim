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

  # Check if the compiler supports IPO/LTO before enabling it,
  # to avoid build errors on unsupported toolchains.
  check_ipo_supported(
    RESULT RS_IPO_SUPPORTED
    OUTPUT RS_IPO_OUTPUT
  )

  if(RS_IPO_SUPPORTED)
    set_property(TARGET ${target_name} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
  else()
    message(WARNING
      "IPO/LTO is not supported for target '${target_name}': "
      "${RS_IPO_OUTPUT}"
    )
  endif()

endfunction()
