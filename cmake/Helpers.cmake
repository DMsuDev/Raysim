# ==============================================================================
#  RAYSIM - PROJECT-WIDE CMAKE UTILITY FUNCTIONS
# ==============================================================================
#  Description: Contains utility functions that can be used throughout the project.
#
#  Copyright (c) 2026 Dayron Mustelier (@DMsuDev)
#  Licensed under the Apache License, Version 2.0.
# ==============================================================================

include_guard()

# ===========================================================================
# rs_third_party_setup(<target>)
#
# Applies a "no warnings" policy to a third-party target so its diagnostics
# do not pollute the build output.
#
# What it does:
#   - Suppresses all compiler warnings (PRIVATE, so they don't propagate)
#   - Disables "treat warnings as errors" for this target
#
# Usage:
#   add_library(some_lib STATIC ...)
#   rs_third_party_setup(some_lib)
# ===========================================================================

function(rs_third_party_setup target)
  if(NOT TARGET "${target}")
    message(FATAL_ERROR
      "[rs] rs_third_party_setup: target '${target}' does not exist.")
  endif()

  # INTERFACE targets only accept INTERFACE properties — PRIVATE is invalid.
  get_target_property(_target_type "${target}" TYPE)

  if(_target_type STREQUAL "INTERFACE_LIBRARY")
    set(_scope INTERFACE)
  else()
    set(_scope PRIVATE)
  endif()

  if(MSVC)
    target_compile_options("${target}" ${_scope}
      /W0         # Disable all warnings
      /WX-        # Disable "warnings as errors" (overrides /WX if set globally)
    )
  else()
    target_compile_options("${target}" ${_scope}
      -w          # Disable all warnings
      -Wno-error  # Disable "warnings as errors" (overrides -Werror if set globally)
    )
  endif()
endfunction()
