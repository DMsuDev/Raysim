# ================================================
# Code coverage configuration (gcov/llvm-cov)
# ================================================

include_guard()

function(enable_coverage target_name)
  if(NOT TARGET ${target_name})
    message(FATAL_ERROR "Target '${target_name}' does not exist")
  endif()

  if(NOT RS_ENABLE_COVERAGE)
    message(STATUS "Coverage: Disabled")
    return()
  endif()

  message(STATUS "Coverage: Enabled")

  # Check if the compiler supports coverage flags
  if(NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    message(WARNING "Coverage is only supported on GCC and Clang")
    return()
  endif()

  # Coverage and sanitizers can interfere with each other, so warn if both are enabled
  if(RS_ENABLE_SANITIZERS)
    message(WARNING "Coverage + Sanitizers may produce unreliable results")
  endif()

  # Add coverage flags to the target
  target_compile_options(${target_name} INTERFACE --coverage -O0 -g)
  target_link_options(${target_name} INTERFACE --coverage)

endfunction()
