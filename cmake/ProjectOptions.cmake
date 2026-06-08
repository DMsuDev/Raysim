# ===========================================================================
# All user-facing build options, early validation, and project-wide helpers.
# Must be included before any find_package() or add_subdirectory() calls.
# ===========================================================================

include_guard()

# ===========================================================================
# Feature flags
# ===========================================================================

option(RS_ENABLE_SANITIZERS  "Enable all sanitizers (ASan + UBSan)"  OFF)
option(RS_ENABLE_COVERAGE    "Enable coverage reporting"              OFF)
option(RS_WARNINGS_AS_ERRORS "Treat compiler warnings as errors"      OFF)
option(RS_ENABLE_LTO         "Enable Link Time Optimization"          OFF)
option(RS_ENABLE_PCH         "Enable precompiled headers"             ON)
option(RS_BUILD_EXAMPLES     "Build example applications"             ON)
option(RS_BUILD_SHARED       "Build Raysim as a shared library"       OFF)
option(RS_ENABLE_PROFILE     "Enable profiling instrumentation"       OFF)

# Individual sanitizers: can also be activated via RS_ENABLE_SANITIZERS.
option(RS_SANITIZE_ASAN  "Enable AddressSanitizer"                OFF)
option(RS_SANITIZE_UBSAN "Enable UndefinedBehaviorSanitizer"      OFF)
option(RS_SANITIZE_TSAN  "Enable ThreadSanitizer"                 OFF)
option(RS_SANITIZE_MSAN  "Enable MemorySanitizer (Clang only)"    OFF)
option(RS_SANITIZE_LSAN  "Enable LeakSanitizer"                   OFF)

# ===========================================================================
# Backend selection
# ===========================================================================

set(_RS_AVAILABLE_BACKENDS raylib glfw_opengl sfml sdl2)

set(RS_BACKEND "raylib" CACHE STRING "Windowing/rendering backend to use")
set_property(CACHE RS_BACKEND PROPERTY STRINGS ${_RS_AVAILABLE_BACKENDS})

# Fail immediately before any find_package() or backend file is touched.
if(NOT RS_BACKEND IN_LIST _RS_AVAILABLE_BACKENDS)
  message(FATAL_ERROR
    "[rs] RS_BACKEND='${RS_BACKEND}' is not supported.\n"
    "     Available: ${_RS_AVAILABLE_BACKENDS}\n"
    "     Usage   : cmake -DRS_BACKEND=<name> .")
endif()

# ---------------------------------------------------------------------------
# Backend preprocessor definitions
# Consumed by source code via #ifdef RS_BACKEND_RAYLIB, etc.
# Keep in sync with _RS_AVAILABLE_BACKENDS above.
# ---------------------------------------------------------------------------

set(_RS_BACKEND_DEFINES
  raylib      RS_BACKEND_RAYLIB
  glfw_opengl RS_BACKEND_GLFW_OPENGL
  sfml        RS_BACKEND_SFML
  sdl2        RS_BACKEND_SDL2
)

list(FIND _RS_BACKEND_DEFINES "${RS_BACKEND}" _idx)
if(_idx GREATER_EQUAL 0)
  math(EXPR _def_idx "${_idx} + 1")
  list(GET _RS_BACKEND_DEFINES ${_def_idx} _backend_define)
  add_compile_definitions(${_backend_define})
  unset(_idx)
  unset(_def_idx)
  unset(_backend_define)
endif()

# ===========================================================================
# Sanitizer consistency
# ===========================================================================

# RS_ENABLE_SANITIZERS=ON is a convenience shortcut: enables ASan + UBSan.
if(RS_ENABLE_SANITIZERS)
  set(RS_SANITIZE_ASAN  ON CACHE BOOL "" FORCE)
  set(RS_SANITIZE_UBSAN ON CACHE BOOL "" FORCE)
endif()

# Reverse sync: if any individual sanitizer is ON, raise the master flag so
# the rest of the build system has a single variable to query.
if(RS_SANITIZE_ASAN OR RS_SANITIZE_UBSAN OR RS_SANITIZE_TSAN OR
  RS_SANITIZE_MSAN OR RS_SANITIZE_LSAN)
  if(NOT RS_ENABLE_SANITIZERS)
    set(RS_ENABLE_SANITIZERS ON CACHE BOOL "" FORCE)
    message(STATUS
      "[rs] RS_ENABLE_SANITIZERS set to ON automatically "
      "(one or more individual sanitizers are enabled).")
  endif()
endif()

# MSan is Clang-only.
if(RS_SANITIZE_MSAN AND NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  message(FATAL_ERROR
    "[rs] RS_SANITIZE_MSAN=ON requires Clang. "
    "Current compiler: ${CMAKE_CXX_COMPILER_ID}")
endif()

# TSan and ASan are mutually exclusive.
if(RS_SANITIZE_TSAN AND RS_SANITIZE_ASAN)
  message(FATAL_ERROR
    "[rs] RS_SANITIZE_TSAN and RS_SANITIZE_ASAN cannot be enabled simultaneously.")
endif()

# LTO is incompatible with sanitizers.
if(RS_ENABLE_LTO AND RS_ENABLE_SANITIZERS)
  message(WARNING "[rs] RS_ENABLE_LTO disabled automatically (sanitizers are ON).")
  set(RS_ENABLE_LTO OFF CACHE BOOL "" FORCE)
endif()

# Coverage and LTO don't mix well either.
if(RS_ENABLE_COVERAGE AND RS_ENABLE_LTO)
  message(WARNING "[rs] RS_ENABLE_LTO disabled automatically (coverage is ON).")
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
# Configuration summary  (top-level builds only)
# ===========================================================================

if(PROJECT_IS_TOP_LEVEL)
  message(STATUS "[rs] -----------------------------------------------")
  message(STATUS "[rs] Configuration summary")
  message(STATUS "[rs] -----------------------------------------------")
  message(STATUS "[rs]   Backend          : ${RS_BACKEND}")
  message(STATUS "[rs]   Library type     : ${RS_LIB_TYPE}")
  message(STATUS "[rs]   Build examples   : ${RS_BUILD_EXAMPLES}")
  message(STATUS "[rs]   PCH              : ${RS_ENABLE_PCH}")
  message(STATUS "[rs]   LTO              : ${RS_ENABLE_LTO}")
  message(STATUS "[rs]   Coverage         : ${RS_ENABLE_COVERAGE}")
  message(STATUS "[rs]   Profiling        : ${RS_ENABLE_PROFILE}")
  message(STATUS "[rs]   Warnings->errors : ${RS_WARNINGS_AS_ERRORS}")
  message(STATUS "[rs]   Sanitizers       : ${RS_ENABLE_SANITIZERS}")
  if(RS_ENABLE_SANITIZERS)
    message(STATUS "[rs]     ASan           : ${RS_SANITIZE_ASAN}")
    message(STATUS "[rs]     UBSan          : ${RS_SANITIZE_UBSAN}")
    message(STATUS "[rs]     TSan           : ${RS_SANITIZE_TSAN}")
    message(STATUS "[rs]     MSan           : ${RS_SANITIZE_MSAN}")
    message(STATUS "[rs]     LSan           : ${RS_SANITIZE_LSAN}")
  endif()
  if(MSVC)
    message(STATUS "[rs]   Compiler         : MSVC ${CMAKE_CXX_COMPILER_VERSION}")
  else()
    message(STATUS "[rs]   Compiler         : ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
  endif()
  message(STATUS "[rs] -----------------------------------------------")
endif()

# ===========================================================================
# rs_apply_build_defines(<target>)
#
# Applies build-profile preprocessor definitions to <target>.
# Call this on every first-party target after it is created.
#
# Definitions added:
#   RS_BUILD_DEBUG      -> Debug and RelWithDebInfo configs
#   RS_ENABLE_PROFILE   -> when RS_ENABLE_PROFILE=ON (same configs)
# ===========================================================================

function(rs_apply_build_defines target)
  if(NOT TARGET "${target}")
    message(FATAL_ERROR
      "[rs] rs_apply_build_defines: target '${target}' does not exist.")
  endif()

  target_compile_definitions("${target}" PUBLIC
    $<$<CONFIG:Debug,RelWithDebInfo>:RS_BUILD_DEBUG>
  )

  if(RS_ENABLE_PROFILE)
    target_compile_definitions("${target}" PUBLIC
      $<$<CONFIG:Debug,RelWithDebInfo>:RS_ENABLE_PROFILE>
    )
  endif()
endfunction()
