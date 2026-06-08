# ===========================================================================
# Applies global, toolchain-level compiler behaviour that must be consistent
# across every translation unit in the project.
#
# Include once from the top-level CMakeLists.txt, after ProjectOptions.cmake
# and before any add_subdirectory() calls.
#
# What this file does NOT do:
#   - Warning flags  -> Warnings.cmake  (rs_warnings INTERFACE target)
#   - Optimisations  -> Optimizations.cmake  (rs_enable_optimizations())
#   - Sanitizers     -> Sanitizers.cmake
# ===========================================================================

include_guard()

# ---------------------------------------------------------------------------
# MSVC
# ---------------------------------------------------------------------------

if(MSVC)
  add_compile_options(
    /utf-8              # Source and execution charset: UTF-8
    /permissive-        # Strict standards conformance (disables MSVC extensions)
    /Zc:__cplusplus     # __cplusplus reflects the actual standard in use
    /Zc:preprocessor    # Use the conformant preprocessor (replaces legacy one)
    /Zc:inline          # Remove unreferenced COMDAT functions / data
    /MP                 # Multi-processor compilation
    /EHsc               # Standard C++ exception handling only
  )

  add_compile_definitions(
    _CRT_SECURE_NO_WARNINGS     # Suppress MSVC's "use _s variants" noise
    NOMINMAX                    # Prevent <windows.h> from defining min/max macros
    WIN32_LEAN_AND_MEAN         # Exclude rarely-used Windows headers
    VC_EXTRALEAN                # Exclude even more rarely-used Windows headers
  )

  # VS 17.4+ (1940)
  if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "19.40")
    add_compile_options(/Zc:enumTypes) # Strongly type enums (disallow implicit conversions to int)
  endif()

  # VS 17.5+ (1941)
  if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "19.41")
    add_compile_options(/Zc:templateScope) # Fix template parameter shadowing
  endif()

# ---------------------------------------------------------------------------
# GCC / Clang / AppleClang
# ---------------------------------------------------------------------------

else()
  add_compile_options(
    -finput-charset=UTF-8       # Source files are UTF-8
    -fexec-charset=UTF-8        # String literals encoded as UTF-8
    -fvisibility=hidden         # Default symbol visibility: hidden (like MSVC)
    -fvisibility-inlines-hidden # Inline functions also hidden by default
  )
endif()

message(STATUS "[rs] Compiler options configured (${CMAKE_CXX_COMPILER_ID})")
