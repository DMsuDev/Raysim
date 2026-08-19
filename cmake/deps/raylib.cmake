# ============================================================================
#  RAYSIM - DEPENDENCY RESOLVER: RAYLIB
# ============================================================================
#  Description: Resolves the raylib dependency from either a bundled submodule
#               or a vcpkg/system installation, then exposes the canonical
#               alias target raylib for backend modules.
#
#  Copyright (c) 2026 Dayron Mustelier (@DMsuDev)
#  Licensed under the Apache License, Version 2.0.
# ============================================================================

include_guard()

# ============================================================================
#  SETUP RAYLIB DEPENDENCY
# ============================================================================

if(RS_SYSTEM_RAYLIB)

  # --------------------------------------------------------------------------
  # System-installed raylib (vcpkg or system path)
  # --------------------------------------------------------------------------

  find_package(raylib CONFIG REQUIRED)

else()

  # --------------------------------------------------------------------------
  # Bundled path (submodule at third_party/raylib)
  # --------------------------------------------------------------------------

  set(_raylib_src "${PROJECT_SOURCE_DIR}/third_party/raylib")

  if(NOT EXISTS "${_raylib_src}/CMakeLists.txt")
    message(FATAL_ERROR
      "[rs] Using bundled raylib, but the submodule is missing. Please run:\n"
      "> git submodule update --init third_party/raylib")
  endif()

  set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
  add_subdirectory("${_raylib_src}" "third_party/raylib" EXCLUDE_FROM_ALL)

  unset(_raylib_src)

endif()
