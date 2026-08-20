# ==============================================================================
#  RAYSIM - CODE COVERAGE
# ==============================================================================
#  Description: Enables code coverage instrumentation using GCC or Clang.
#
#  Copyright (c) 2026 Dayron Mustelier (@DMsuDev)
#  Licensed under the Apache License, Version 2.0.
# ==============================================================================

include_guard()

# ------------------------------------------------------------------------------
#  Target Coverage Instrumentation
# ------------------------------------------------------------------------------

function(rs_enable_coverage target)
  if(NOT TARGET ${target})
    message(FATAL_ERROR "[rs] Target '${target}' does not exist.")
  endif()

  if(NOT RS_ENABLE_COVERAGE)
    return()
  endif()

  # ----------------------------------------------------------------------------
  #  Compiler Compatibility Check
  # ----------------------------------------------------------------------------
  if(NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    message(WARNING "[rs] Code coverage is only supported with GCC and Clang.")
    return()
  endif()

  # ----------------------------------------------------------------------------
  #  Instrumentation Options
  # ----------------------------------------------------------------------------
  target_compile_options(${target}
    PRIVATE
      -g
      --coverage
  )

  target_link_options(${target}
    PRIVATE
      --coverage
  )
endfunction()
