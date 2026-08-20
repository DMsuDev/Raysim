# ============================================================================
#  RAYSIM - THIRD-PARTY DEPENDENCY MANAGEMENT
# ============================================================================
#  Description: Single entry point for all third-party dependencies used by
#               Raysim. Selects the active backend and loads its dependencies
#               before handing off to the Backend Registry.
#
#  Copyright (c) 2026 Dayron Mustelier (@DMsuDev)
#  Licensed under the Apache License, Version 2.0.
# ============================================================================

include_guard()

message(STATUS "[rs] Backend selected: ${RS_BACKEND}")

# ----------------------------------------------------------------------------
#  Common Dependencies  (required by all backends)
# ----------------------------------------------------------------------------

find_package(Threads REQUIRED)

include(modules/add_spdlog)
include(modules/add_imgui)
include(modules/add_stb)

# ------------------------------------------------------------------------------
#  Backend-Specific Dependencies
# ------------------------------------------------------------------------------

if(RS_BACKEND STREQUAL "raylib")

  include(deps/raylib)

elseif(RS_BACKEND STREQUAL "glfw_opengl")

  message(FATAL_ERROR
    "[rs] Backend 'glfw_opengl' is not yet implemented. Use 'raylib' for now.")

  # include(deps/glfw)
  # include(modules/add_glad)

elseif(RS_BACKEND STREQUAL "sfml")

  message(FATAL_ERROR
    "[rs] Backend 'sfml' is not yet implemented. Use 'raylib' for now.")

elseif(RS_BACKEND STREQUAL "sdl2")

  message(FATAL_ERROR
    "[rs] Backend 'sdl2' is not yet implemented. Use 'raylib' for now.")

endif()

# ---------------------------------------------------------------------------
# Backend Registry (windowing + graphics + ImGui binding)
# ---------------------------------------------------------------------------

include(Backends/BackendRegistry)
