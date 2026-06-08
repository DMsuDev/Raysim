# ===========================================================================
# Detects the host OS, sets platform variables and preprocessor definitions,
# and validates that the selected RS_BACKEND is supported on this platform.
#
# Output variables (all BOOL, exactly one is ON):
#   RS_PLATFORM_WINDOWS
#   RS_PLATFORM_MACOS
#   RS_PLATFORM_LINUX
#
# Preprocessor definitions added globally:
#   RS_PLATFORM_WINDOWS / RS_PLATFORM_MACOS / RS_PLATFORM_LINUX
# ===========================================================================

include_guard()

# ---------------------------------------------------------------------------
# Detect platform
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
    "[rs] PlatformDetection: unsupported platform.\n"
    "CMAKE_SYSTEM_NAME = '${CMAKE_SYSTEM_NAME}'")
endif()

# ---------------------------------------------------------------------------
# Backend / platform compatibility matrix
#
# Add rows here when a new backend or platform constraint is introduced.
# Format: "<backend>;<platform_var>" the pair is UNSUPPORTED.
# ---------------------------------------------------------------------------

set(_RS_UNSUPPORTED_COMBINATIONS
  # MemorySanitizer is Clang-only and unavailable on Windows
  # (not a backend, but kept here as a model for future constraints)
)

# Backend-level restrictions
if(RS_PLATFORM_WINDOWS)
  # glfw_opengl works on Windows but MSan is unsupported; no backend
  # restrictions currently - placeholder kept for future use.
elseif(RS_PLATFORM_MACOS)
  if(RS_BACKEND STREQUAL "glfw_opengl")
    message(WARNING
      "[rs] PlatformDetection: backend 'glfw_opengl' on macOS requires "
      "the OpenGL Compatibility profile. Consider 'raylib' instead.")
  endif()
endif()

# ---------------------------------------------------------------------------
# Summary
# ---------------------------------------------------------------------------

message(STATUS "[rs] Platform detected: ${_RS_PLATFORM_NAME}")
