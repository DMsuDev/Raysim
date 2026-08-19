# ============================================================================
#  RAYSIM - THIRD-PARTY DEPENDENCY MANAGEMENT
# ============================================================================
#  Description: Single entry point for all third-party dependencies used by Raysim.
#
#  Copyright (c) 2026 Dayron Mustelier (@DMsuDev)
#  Licensed under the Apache License, Version 2.0.
# ============================================================================

include_guard()

# ----------------------------------------------------------------------------
# Backend selection  (single source of truth for RS_BACKEND)
# ----------------------------------------------------------------------------

set(_RS_AVAILABLE_BACKENDS
  raylib
  glfw_opengl
  sfml
  sdl2
)

set(RS_BACKEND "raylib"
  CACHE STRING "Windowing/rendering backend to use"
)

set_property(CACHE RS_BACKEND PROPERTY STRINGS ${_RS_AVAILABLE_BACKENDS})

if(NOT RS_BACKEND IN_LIST _RS_AVAILABLE_BACKENDS)
  message(FATAL_ERROR
    "[rs] Unknown backend '${RS_BACKEND}'.\n"
    "     Available: ${_RS_AVAILABLE_BACKENDS}\n"
    "     Set via: cmake -DRS_BACKEND=<name> .")
endif()

message(STATUS "[rs] Backend selected: ${RS_BACKEND}")

# ----------------------------------------------------------------------------
# Common deps  (all backends need these)
# ----------------------------------------------------------------------------

find_package(Threads REQUIRED)

include(modules/add_spdlog)
include(modules/add_imgui)
include(modules/add_stb)

# ----------------------------------------------------------------------------
# Backend-specific deps
# ----------------------------------------------------------------------------
# Rules:
#   - Include deps/ before modules/ — modules may depend on dep targets.
#   - Include everything the backend needs BEFORE BackendRegistry,
#     so backend cmake files can assume all targets already exist.
#   - OpenGL is always a system dep; backends that need it call
#     find_package(OpenGL) themselves — no bundled variant exists.
# ----------------------------------------------------------------------------

if(RS_BACKEND STREQUAL "raylib")

  include(deps/raylib)

elseif(RS_BACKEND STREQUAL "glfw_opengl")

  message(FATAL_ERROR
    "[rs] Backend 'glfw_opengl' is not yet supported.\n"
    "     Please use raylib for now.")

  # include(deps/glfw)
  # include(modules/add_glad)

elseif(RS_BACKEND STREQUAL "sfml")

  message(FATAL_ERROR
    "[rs] Backend 'sfml' is not yet supported.\n"
    "     Please use raylib for now.")

elseif(RS_BACKEND STREQUAL "sdl2")

  message(FATAL_ERROR
    "[rs] Backend 'sdl2' is not yet supported.\n"
    "     Please use raylib for now.")

endif()

# ---------------------------------------------------------------------------
# Backend Registry (windowing + graphics + ImGui binding)
# ---------------------------------------------------------------------------

include(Backends/BackendRegistry)
