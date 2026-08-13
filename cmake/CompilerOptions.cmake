# ==============================================================================
#  RAYSIM - COMPILER OPTIONS
# ==============================================================================
#  Description: Configures correctness, conformance, and hardening flags for
#               CMake targets.
#
#  Copyright (c) 2026 @DMsuDev. Licensed under the Apache License, Version 2.0.
#  See LICENSE file in the project root for full license text.
# ==============================================================================

include_guard()

# ------------------------------------------------------------------------------
#  rs_set_compiler_options(<target>)
# ------------------------------------------------------------------------------
#  Applies Raysim's correctness and conformance policy to a single target with
#  PRIVATE scope. Call once per library or executable target after it is
#  declared.
# ------------------------------------------------------------------------------
function(rs_set_compiler_options target)
  if(NOT TARGET ${target})
    message(FATAL_ERROR "[rs] Target '${target}' does not exist. Cannot apply compiler options.")
  endif()

  # ----------------------------------------------------------------------------
  # MSVC
  # ----------------------------------------------------------------------------
  if(MSVC)
    target_compile_options(${target} PRIVATE
      /utf-8            # Source and execution charset: UTF-8
      /permissive-      # Strict standards conformance (disables MSVC extensions)
      /Zc:__cplusplus   # __cplusplus reflects the actual standard in use
      /Zc:preprocessor  # Conformant preprocessor (matches GCC/Clang expansion rules)
      /Zc:inline        # Remove unreferenced COMDAT functions / data
      /MP               # Multi-processor compilation
      /EHsc             # Standard C++ exception handling only
    )

    target_compile_definitions(${target} PRIVATE
      _CRT_SECURE_NO_WARNINGS  # Suppress MSVC's "use _s variants" noise
      NOMINMAX                 # Prevent <windows.h> from defining min/max macros
      WIN32_LEAN_AND_MEAN      # Exclude rarely-used Windows headers
      VC_EXTRALEAN             # Exclude even more rarely-used Windows headers
    )

    # VS 17.4+ (cl 19.40)
    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "19.40")
      target_compile_options(${target} PRIVATE
        /Zc:enumTypes             # Strongly type enums (disallow implicit int conversions)
      )
    endif()

    # VS 17.5+ (cl 19.41)
    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "19.41")
      target_compile_options(${target} PRIVATE
        /Zc:templateScope         # Fix template parameter shadowing
      )
    endif()

  # ----------------------------------------------------------------------------
  # GCC / Clang / AppleClang
  # ----------------------------------------------------------------------------
  else()
    target_compile_options(${target} PRIVATE
      -finput-charset=UTF-8                      # Source files are UTF-8
      -fexec-charset=UTF-8                       # String literals encoded as UTF-8
      $<$<CONFIG:Release>:-D_FORTIFY_SOURCE=2>   # Buffer overflow checks in libc calls
    )
  endif()

endfunction()
