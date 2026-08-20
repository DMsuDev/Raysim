# ==============================================================================
#  RAYSIM - EXAMPLE HELPERS
# ==============================================================================
#  Description: Shared CMake utilities for Raysim example targets.
#               Handles compiler options, sanitizers, IDE folder
#               organization, and platform metadata (Windows .rc,
#               Linux .desktop) in a single call.
#
#  Public API:
#    rs_example_setup(<target>
#      NAME        <display name>
#      DESCRIPTION <short description>   [optional]
#    )
#
#  Copyright (c) 2026 Dayron Mustelier (@DMsuDev)
#  Licensed under the Apache License, Version 2.0.
# ==============================================================================

include_guard()

set(_RS_RESOURCES_DIR "${CMAKE_CURRENT_LIST_DIR}/..")
set(_RS_PLATFORM_DIR  "${_RS_RESOURCES_DIR}/platform")
set(_RS_ICONS_DIR     "${_RS_RESOURCES_DIR}/assets/icons")

# ==============================================================================
#  rs_example_setup(<target> NAME <name> [DESCRIPTION <desc>])
# ==============================================================================
function(rs_example_setup target)
  cmake_parse_arguments(PARSE_ARGV 1 _EX "" "NAME;DESCRIPTION" "")

  if(NOT _EX_NAME)
    message(FATAL_ERROR "[rs_example_setup] NAME is required.")
  endif()

  if(NOT _EX_DESCRIPTION)
    set(_EX_DESCRIPTION "${_EX_NAME} — Raysim example")
  endif()

  if(NOT TARGET "${target}")
    message(FATAL_ERROR "[rs_example_setup] Target '${target}' does not exist.")
  endif()

  # --------------------------------------------------------------------------
  #  Common setup
  # --------------------------------------------------------------------------
  rs_set_compiler_options("${target}")
  sanitizers_enable("${target}")

  # IDE folder organization (Visual Studio / CLion)
  set_target_properties("${target}" PROPERTIES FOLDER "Examples/${target}")

  # --------------------------------------------------------------------------
  #  Windows: embed icon + version metadata via .rc
  # --------------------------------------------------------------------------
  if(WIN32)
    set(RAYSIM_EXAMPLE_NAME        "${_EX_NAME}")
    set(RAYSIM_EXAMPLE_DESCRIPTION "${_EX_DESCRIPTION}")
    set(RAYSIM_EXAMPLE_TARGET      "${target}")
    set(RAYSIM_AUTHOR              "Dayron Mustelier (@DMsuDev)")
    set(RAYSIM_COPYRIGHT_YEAR      "2026")

    # Normalize icon path to native Windows format (rc.exe requires backslashes)
    file(TO_NATIVE_PATH "${_RS_ICONS_DIR}/raysim.ico" _icon_native)
    string(REPLACE "\\" "\\\\" RAYSIM_EXAMPLE_ICON_WIN "${_icon_native}")

    set(_rc_out "${CMAKE_CURRENT_BINARY_DIR}/${target}_example.rc")

    configure_file(
      "${_RS_PLATFORM_DIR}/example.rc.in"
      "${_rc_out}"
      @ONLY
    )

    target_sources("${target}" PRIVATE "${_rc_out}")
  endif()

  # --------------------------------------------------------------------------
  #  Linux: generate .desktop entry
  # --------------------------------------------------------------------------
  if(UNIX AND NOT APPLE)
    set(RAYSIM_EXAMPLE_NAME        "${_EX_NAME}")
    set(RAYSIM_EXAMPLE_DESCRIPTION "${_EX_DESCRIPTION}")
    set(RAYSIM_EXAMPLE_TARGET      "${target}")

    set(_desktop_out "${CMAKE_CURRENT_BINARY_DIR}/${target}.desktop")

    configure_file(
      "${_RS_PLATFORM_DIR}/example.desktop.in"
      "${_desktop_out}"
      @ONLY
    )
  endif()

endfunction()
