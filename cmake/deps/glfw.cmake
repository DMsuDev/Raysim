# ============================================================================
#  RAYSIM - DEPENDENCY RESOLVER: GLFW
# ============================================================================
#  Description: Resolves the GLFW dependency from either a bundled submodule
#               or a vcpkg/system installation, then exposes the canonical
#               alias target glfw for backend modules.
#
#  Copyright (c) 2026 Dayron Mustelier (@DMsuDev)
#  Licensed under the Apache License, Version 2.0.
# ============================================================================

include_guard()

# ============================================================================
#  SETUP GLFW DEPENDENCY
# ============================================================================

if(RS_SYSTEM_GLFW)

  # --------------------------------------------------------------------------
  # System-installed GLFW (vcpkg or system path)
  # --------------------------------------------------------------------------

  find_package(glfw3 CONFIG REQUIRED)

else()

  # --------------------------------------------------------------------------
  # Bundled path (submodule at third_party/glfw)
  # --------------------------------------------------------------------------

  set(_glfw_src "${PROJECT_SOURCE_DIR}/third_party/glfw")

  if(NOT EXISTS "${_glfw_src}/CMakeLists.txt")
    message(FATAL_ERROR
      "[rs] Using bundled GLFW, but the submodule is missing. Please run:\n"
      "> git submodule update --init third_party/glfw")
  endif()

  # Disable extras not needed when GLFW is built as a dependency.
  set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
  set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
  set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
  set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)

  add_subdirectory("${_glfw_src}" "third_party/glfw" EXCLUDE_FROM_ALL)

  unset(_glfw_src)

endif()
