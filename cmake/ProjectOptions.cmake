# ================================================
# ProjectOptions.cmake
# Centralized project options
# ================================================

include_guard()

# ============================================================
# Global options
# ============================================================

option(RS_ENABLE_SANITIZERS  "Enable all sanitizers" OFF)
option(RS_ENABLE_COVERAGE    "Enable coverage reporting" OFF)
option(RS_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)

# Sanitizers
option(RS_ENABLE_ASAN  "Enable AddressSanitizer" OFF)
option(RS_ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer" OFF)
option(RS_ENABLE_TSAN  "Enable ThreadSanitizer" OFF)
option(RS_ENABLE_MSAN  "Enable MemorySanitizer (Clang only)" OFF)
option(RS_ENABLE_LSAN  "Enable LeakSanitizer" OFF)

# Features
option(RS_ENABLE_LTO     "Enable Link Time Optimization" OFF)
option(RS_ENABLE_PCH     "Enable precompiled headers" ON)
option(RS_BUILD_EXAMPLES "Build example applications" ON)
option(RS_BUILD_SHARED   "Build Raysim as shared library" OFF)
option(RS_ENABLE_PROFILE "Enable profiling instrumentation" OFF)

# ============================================================
# Sanitizer consistency
# ============================================================

if(RS_ENABLE_SANITIZERS)
  set(RS_ENABLE_ASAN ON CACHE BOOL "" FORCE)
  set(RS_ENABLE_UBSAN ON CACHE BOOL "" FORCE)
endif()

# ============================================================
# Conflict handling
# ============================================================

if(RS_ENABLE_LTO AND RS_ENABLE_SANITIZERS)
  message(WARNING "Disabling LTO because sanitizers are enabled")
  set(RS_ENABLE_LTO OFF CACHE BOOL "" FORCE)
endif()

# ============================================================
# Library type
# ============================================================

set(RS_LIB_TYPE STATIC)
if(RS_BUILD_SHARED)
  set(RS_LIB_TYPE SHARED)
endif()

# ============================================================
# Helper function (target-based defines)
# ============================================================

# Helper function to apply build profile defines to a target
function(rs_apply_build_defines target)
  if(NOT TARGET ${target})
    message(FATAL_ERROR "Target '${target}' does not exist")
  endif()

  target_compile_definitions(${target}
    PUBLIC
      $<$<CONFIG:Debug,RelWithDebInfo>:RS_BUILD_DEBUG>
  )

  if(RS_ENABLE_PROFILE)
    target_compile_definitions(${target}
      PUBLIC
        $<$<CONFIG:Debug,RelWithDebInfo,Release>:RS_ENABLE_PROFILE>
    )
  endif()
endfunction()
