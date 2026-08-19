# ============================================================================
#  RAYSIM - BACKEND REGISTRY
# ============================================================================
#  Description: Selects, validates, and loads the active windowing/rendering
#               backend. After include(), the following targets are available:
#
#  Responsibilities of this file:
#    - Derive the C++ preprocessor define for the active backend.
#    - Locate and include the backend cmake file.
#    - Validate that the backend created the required targets.
#    - Create the rs::backend meta-target.
#
#  After include(), the following targets are available:
#    rs::windowing      - windowing + input (INTERFACE)
#    rs::graphics       - graphics API glue (INTERFACE)
#    rs::imgui_backend  - ImGui platform/renderer binding (STATIC)
#    rs::backend        - meta-target linking all three (INTERFACE)
#
#  Copyright (c) 2026 Dayron Mustelier (@DMsuDev)
#  Licensed under the Apache License, Version 2.0.
# ============================================================================

include_guard()

if(NOT DEFINED RS_BACKEND)
  message(FATAL_ERROR
    "[rs] BackendRegistry: RS_BACKEND is not defined.\n"
    "     Include ThirdParty.cmake instead of BackendRegistry directly.")
endif()

# ----------------------------------------------------------------------------
# Preprocessor define
# ----------------------------------------------------------------------------

# Derive a C++ preprocessor definition from RS_BACKEND (e.g. raylib -> RS_BACKEND_RAYLIB)
string(TOUPPER  "${RS_BACKEND}" _backend_define)
string(REPLACE  "-" "_" _backend_define "${_backend_define}")
add_compile_definitions("RS_BACKEND_${_backend_define}")
unset(_backend_define)

# ----------------------------------------------------------------------------
# Load backend file
# ----------------------------------------------------------------------------

set(_backend_file "${CMAKE_CURRENT_LIST_DIR}/${RS_BACKEND}.cmake")

if(NOT EXISTS "${_backend_file}")
  message(FATAL_ERROR
    "[rs] BackendRegistry: no implementation found for '${RS_BACKEND}'.\n"
    "     Expected: ${_backend_file}\n"
    "     Create cmake/Backends/${RS_BACKEND}.cmake to add support.")
endif()

include("${_backend_file}")
unset(_backend_file)

# ----------------------------------------------------------------------------
# Post-load validation
# ----------------------------------------------------------------------------

foreach(_required_target IN ITEMS rs::windowing rs::graphics rs::imgui_backend)
  if(NOT TARGET ${_required_target})
    message(FATAL_ERROR
      "[rs] BackendRegistry: backend '${RS_BACKEND}' did not create '${_required_target}'.\n"
      "     Every backend cmake file must define:\n"
      "       rs::windowing      - windowing/input INTERFACE target\n"
      "       rs::graphics       - graphics API INTERFACE target\n"
      "       rs::imgui_backend  - ImGui glue STATIC target (ALIAS)"
    )
  endif()
endforeach()

# ----------------------------------------------------------------------------
# Meta-target: rs::backend
# ----------------------------------------------------------------------------

# Single target to link all three backend targets at once.
add_library(rs_backend  INTERFACE)
add_library(rs::backend ALIAS rs_backend)

target_link_libraries(rs_backend INTERFACE
  rs::windowing
  rs::graphics
  rs::imgui_backend
)

set_target_properties(rs_backend PROPERTIES FOLDER "Backends")

message(STATUS "[rs] Backend registry: '${RS_BACKEND}' loaded successfully")
