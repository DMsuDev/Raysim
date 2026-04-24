# ================================================
# Code coverage configuration (gcov/llvm-cov)
# ================================================

include_guard()

function(rs_enable_coverage target_name)
  if(NOT TARGET ${target_name})
    message(FATAL_ERROR "Target '${target_name}' does not exist")
  endif()

  if(NOT RS_ENABLE_COVERAGE)
    return()
  endif()

  # Check if the compiler supports coverage flags
  if(NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    message(WARNING "Coverage is only supported on GCC and Clang")
    return()
  endif()

  # Coverage and sanitizers can interfere with each other, so warn if both are enabled
  if(RS_ENABLE_SANITIZERS)
    message(WARNING "Coverage + Sanitizers may produce unreliable results")
  endif()

  message(STATUS "Coverage enabled for ${target_name}")

  # Flags for coverage instrumentation
  set(COVERAGE_COMPILE_FLAGS
    -O0
    -g
    --coverage
  )

  set(COVERAGE_LINK_FLAGS
    --coverage
  )

  # Add coverage flags to the target
  target_compile_options(${target_name} PRIVATE
    $<$<CONFIG:Debug,RelWithDebInfo>:${COVERAGE_COMPILE_FLAGS}>
  )
  target_link_options(${target_name} PRIVATE
    $<$<CONFIG:Debug,RelWithDebInfo>:${COVERAGE_LINK_FLAGS}>
  )

endfunction()
