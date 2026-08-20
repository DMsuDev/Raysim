# ==============================================================================
#  RAYSIM - LINK TIME OPTIMIZATION (LTO / IPO)
# ==============================================================================
#  Description: Enables Link Time Optimization for Release and RelWithDebInfo
#               configurations.
#
#  Copyright (c) 2026 Dayron Mustelier (@DMsuDev)
#  Licensed under the Apache License, Version 2.0.
# ==============================================================================

include_guard()

function(rs_enable_lto target_name)

  if(NOT TARGET "${target_name}")
    message(FATAL_ERROR
      "[rs] LTO: target '${target_name}' does not exist."
    )
  endif()

  if(NOT RS_ENABLE_LTO)
    return()
  endif()

  include(CheckIPOSupported)

  check_ipo_supported(
    RESULT _ipo_supported
    OUTPUT _ipo_output
    LANGUAGES CXX
  )

  if(NOT _ipo_supported)
    message(WARNING
      "[rs] LTO: IPO is not supported by this toolchain — "
      "skipping '${target_name}'.\n"
      "Details: ${_ipo_output}"
    )

    set("RS_LTO_ENABLED_FOR_${target_name}" FALSE PARENT_SCOPE)
    return()
  endif()

  set_target_properties("${target_name}" PROPERTIES
    INTERPROCEDURAL_OPTIMIZATION                FALSE
    INTERPROCEDURAL_OPTIMIZATION_DEBUG          FALSE
    INTERPROCEDURAL_OPTIMIZATION_MINSIZEREL     FALSE
    INTERPROCEDURAL_OPTIMIZATION_RELEASE        TRUE
    INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO TRUE
  )

  set("RS_LTO_ENABLED_FOR_${target_name}" TRUE PARENT_SCOPE)

endfunction()
