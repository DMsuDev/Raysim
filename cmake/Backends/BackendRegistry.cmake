# ============================================================================
#  RAYSIM - BACKEND REGISTRY MODULE
# ============================================================================
#  Description: Dynamic resolution and initialization module for windowing and
#               rendering backends (Raylib, GLFW, SFML, SDL2).
#               Configures compile definitions, links backend targets, and
#               exports the 'rs::backend' meta-interface target.
#
#  Copyright (c) 2026 Dayron Mustelier (@DMsuDev)
#  Licensed under the Apache License, Version 2.0.
# ============================================================================

include_guard()

# ---------------------------------------------------------------------------
# Backend selection
# ---------------------------------------------------------------------------

set(_RS_AVAILABLE_BACKENDS
  raylib
  glfw_opengl
  sfml
  sdl2
)

# Default backend is raylib, but can be overridden by the user via -DRS_BACKEND=<name>
set(RS_BACKEND "raylib"
  CACHE STRING "Windowing/rendering backend to use"
)

set_property(CACHE RS_BACKEND PROPERTY STRINGS ${_RS_AVAILABLE_BACKENDS})

if(NOT RS_BACKEND IN_LIST _RS_AVAILABLE_BACKENDS)
  message(FATAL_ERROR
    "[rs] Unsupported backend: '${RS_BACKEND}'.\n"
    "     Available backends: ${_RS_AVAILABLE_BACKENDS}\n"
    "     Usage: cmake -DRS_BACKEND=<name> ."
  )
endif()

# ---------------------------------------------------------------------------
# Resolve backend file
# ---------------------------------------------------------------------------

set(_backend_file "${CMAKE_CURRENT_LIST_DIR}/${RS_BACKEND}.cmake")

if(NOT EXISTS "${_backend_file}")
  message(FATAL_ERROR
    "[rs] BackendRegistry: no implementation file found for backend '${RS_BACKEND}'.\n"
    "     Expected path  : ${_backend_file}\n"
    "     To add support : create cmake/Backends/${RS_BACKEND}.cmake"
  )
endif()

# ---------------------------------------------------------------------------
# Backend preprocessor definition
#
# Derived automatically from RS_BACKEND — no lookup table needed.
# glfw_opengl -> RS_BACKEND_GLFW_OPENGL, raylib -> RS_BACKEND_RAYLIB, etc.
# ---------------------------------------------------------------------------

string(TOUPPER    "${RS_BACKEND}" _backend_define)
string(REPLACE "-" "_" _backend_define "${_backend_define}")
add_compile_definitions("RS_BACKEND_${_backend_define}")
unset(_backend_define)

# ---------------------------------------------------------------------------
# Load backend
# ---------------------------------------------------------------------------

include("${_backend_file}")

# ---------------------------------------------------------------------------
# Post-load validation
# Make sure the backend file set the two mandatory output variables.
# ---------------------------------------------------------------------------

foreach(_var IN ITEMS RS_IMGUI_BACKEND_TARGET RS_IMGUI_BACKEND_EXTRA_INCLUDES)
  if(NOT DEFINED ${_var} OR "${${_var}}" STREQUAL "")
    message(FATAL_ERROR
      "[rs] BackendRegistry: backend '${RS_BACKEND}' did not set '${_var}'.\n"
      "     Every backend cmake file must define:\n"
      "       RS_IMGUI_BACKEND_TARGET          -> CMake target to link\n"
      "       RS_IMGUI_BACKEND_EXTRA_INCLUDES  -> additional include dirs for the engine"
    )
  endif()
endforeach()

# ---------------------------------------------------------------------------
# Meta-target: rs::backend
# ---------------------------------------------------------------------------

add_library(rs_backend  INTERFACE)
add_library(rs::backend ALIAS rs_backend)

target_link_libraries(rs_backend INTERFACE
  rs::windowing
  rs::graphics
  "${RS_IMGUI_BACKEND_TARGET}"
)

target_include_directories(rs_backend SYSTEM INTERFACE
  ${RS_IMGUI_BACKEND_EXTRA_INCLUDES}
)

set_target_properties(rs_backend PROPERTIES FOLDER "Backends")

message(STATUS "[rs] Backend registry: loaded '${RS_BACKEND}'")
