# ============================================================================
#  RAYSIM - PLATFORM DETECTION MODULE
# ============================================================================
#  Description: Detects the host OS, sets RS_PLATFORM_* CMake variables,
#               injects the corresponding preprocessor definition into all
#               targets, and validates backend/platform compatibility.
#
#  Output variables (BOOL, exactly one is ON):
#    RS_PLATFORM_WINDOWS
#    RS_PLATFORM_MACOS
#    RS_PLATFORM_LINUX
#
#  Preprocessor definition injected (matches the ON variable):
#    RS_PLATFORM_WINDOWS | RS_PLATFORM_MACOS | RS_PLATFORM_LINUX
#
#  Copyright (c) 2026 Dayron Mustelier (@DMsuDev)
#  Licensed under the Apache License, Version 2.0.
# ============================================================================
include_guard()

# ---------------------------------------------------------------------------
#  Detect platform
# ---------------------------------------------------------------------------

if(WIN32)
  set(RS_PLATFORM_WINDOWS ON)
  set(RS_PLATFORM_MACOS   OFF)
  set(RS_PLATFORM_LINUX   OFF)
  set(_RS_PLATFORM_NAME   "Windows")
  add_compile_definitions(RS_PLATFORM_WINDOWS)

elseif(APPLE)
  set(RS_PLATFORM_WINDOWS OFF)
  set(RS_PLATFORM_MACOS   ON)
  set(RS_PLATFORM_LINUX   OFF)
  set(_RS_PLATFORM_NAME   "macOS")
  add_compile_definitions(RS_PLATFORM_MACOS)

elseif(UNIX)
  set(RS_PLATFORM_WINDOWS OFF)
  set(RS_PLATFORM_MACOS   OFF)
  set(RS_PLATFORM_LINUX   ON)
  set(_RS_PLATFORM_NAME   "Linux")
  add_compile_definitions(RS_PLATFORM_LINUX)

else()
  message(FATAL_ERROR
    "[PlatformDetection] Unsupported platform: '${CMAKE_SYSTEM_NAME}'.\n"
    "Supported targets: Windows x64, Linux x86_64, macOS (experimental).")
endif()

# ------------------------------------------------------------------------------
#  Backend / platform compatibility
# ------------------------------------------------------------------------------
#  Add entries here when a new backend introduces a platform restriction.
#  Warnings are preferred over FATAL_ERROR to allow CI to surface all issues
#  at once rather than aborting on the first mismatch.
# ------------------------------------------------------------------------------

if(RS_PLATFORM_MACOS AND RS_BACKEND STREQUAL "glfw_opengl")
  message(WARNING
    "[PlatformDetection] Backend 'glfw_opengl' on macOS uses OpenGL, which "
    "Apple has deprecated since macOS 10.14 and no longer actively supports. "
    "Functionality may be limited or broken on recent macOS versions. "
    "Consider using the 'raylib' backend instead.")
endif()
