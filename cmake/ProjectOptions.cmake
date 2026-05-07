# ===========================================================================
# ProjectOptions.cmake
# All user-facing build options and early validation.
# Must be included before any find_package() or add_subdirectory() calls.
# ===========================================================================

include_guard()

# ===========================================================================
# Feature flags
# ===========================================================================

option(RS_ENABLE_SANITIZERS  "Enable all sanitizers"               OFF)
option(RS_ENABLE_COVERAGE    "Enable coverage reporting"           OFF)
option(RS_WARNINGS_AS_ERRORS "Treat compiler warnings as errors"   OFF)
option(RS_ENABLE_LTO         "Enable Link Time Optimization"       OFF)
option(RS_ENABLE_PCH         "Enable precompiled headers"          ON)
option(RS_BUILD_EXAMPLES     "Build example applications"          ON)
option(RS_BUILD_SHARED       "Build Raysim as a shared library"    OFF)
option(RS_ENABLE_PROFILE     "Enable profiling instrumentation"    OFF)

# Individual sanitizers (can also be enabled via RS_ENABLE_SANITIZERS)
option(RS_ENABLE_ASAN  "Enable AddressSanitizer"                  OFF)
option(RS_ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer"        OFF)
option(RS_ENABLE_TSAN  "Enable ThreadSanitizer"                   OFF)
option(RS_ENABLE_MSAN  "Enable MemorySanitizer (Clang only)"      OFF)
option(RS_ENABLE_LSAN  "Enable LeakSanitizer"                     OFF)

# ===========================================================================
# Backend selection
# ===========================================================================

set(_RS_AVAILABLE_BACKENDS raylib glfw_opengl sfml sdl2)

set(RS_BACKEND "raylib" CACHE STRING "Windowing/rendering backend to use")
set_property(CACHE RS_BACKEND PROPERTY STRINGS ${_RS_AVAILABLE_BACKENDS})

# Validate early so configure fails immediately with a clear message,
# before any find_package() or backend cmake file is touched.
if(NOT RS_BACKEND IN_LIST _RS_AVAILABLE_BACKENDS)
  message(FATAL_ERROR
    "RS_BACKEND='${RS_BACKEND}' is not supported.\n"
    "Available backends: ${_RS_AVAILABLE_BACKENDS}\n"
    "Set it with: -DRS_BACKEND=<name>")
endif()

# Preprocessor definition consumed by source code (e.g. #ifdef RS_BACKEND_RAYLIB)
if(RS_BACKEND STREQUAL "raylib")
  add_compile_definitions(RS_BACKEND_RAYLIB)
elseif(RS_BACKEND STREQUAL "glfw_opengl")
  add_compile_definitions(RS_BACKEND_GLFW_OPENGL)
endif()

# ===========================================================================
# Sanitizer consistency
# ===========================================================================

# RS_ENABLE_SANITIZERS is a convenience switch that turns on ASAN + UBSAN.
if(RS_ENABLE_SANITIZERS)
  set(RS_ENABLE_ASAN  ON CACHE BOOL "" FORCE)
  set(RS_ENABLE_UBSAN ON CACHE BOOL "" FORCE)
endif()

# LTO is incompatible with sanitizers; disable it automatically.
if(RS_ENABLE_LTO AND RS_ENABLE_SANITIZERS)
  message(WARNING "LTO disabled because sanitizers are enabled.")
  set(RS_ENABLE_LTO OFF CACHE BOOL "" FORCE)
endif()

# ===========================================================================
# Library type
# ===========================================================================

if(RS_BUILD_SHARED)
  set(RS_LIB_TYPE SHARED)
else()
  set(RS_LIB_TYPE STATIC)
endif()

# ===========================================================================
# Summary of options
# ===========================================================================

message(STATUS "====================================")
message(STATUS "  Raysim backend : ${RS_BACKEND}")
message(STATUS "  Library type   : ${RS_LIB_TYPE}")
message(STATUS "====================================")

# ===========================================================================
# rs_apply_build_defines(<target>)
# Applies build-profile compile definitions to a target.
# ===========================================================================

function(rs_apply_build_defines target)
  if(NOT TARGET ${target})
    message(FATAL_ERROR "rs_apply_build_defines: target '${target}' does not exist.")
  endif()

  target_compile_definitions(${target}
    PUBLIC
      $<$<CONFIG:Debug,RelWithDebInfo>:RS_BUILD_DEBUG>
  )

  if(RS_ENABLE_PROFILE)
    target_compile_definitions(${target}
      PUBLIC
        $<$<CONFIG:Debug,RelWithDebInfo>:RS_ENABLE_PROFILE>
    )
  endif()
endfunction()
