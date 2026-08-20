# ============================================================================
#  RAYSIM - ROOT CMAKE CONFIGURATION
# ============================================================================
#  Description: Main CMake configuration for the Raysim project.
#
#  Copyright (c) 2026 Dayron Mustelier (@DMsuDev)
#  Licensed under the Apache License, Version 2.0.
# ============================================================================

cmake_minimum_required(VERSION 3.28)

project(Raysim
  VERSION 0.95.0
  LANGUAGES CXX C
  DESCRIPTION "C++ 2D graphics & simulation framework built on raylib."
)
message(STATUS "Raysim version: ${PROJECT_VERSION}")

# Disable automatic C++ module dependency scanning; Raysim does not use modules.
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

# Standard install directories (bin, lib, include, etc.) with platform-specific defaults.
include(GNUInstallDirs)

# Directory for easier includes
# Anywhere you see include(...) you can check <root>/cmake for that file
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# ------------------------------------------------------------------------------
#  Project-Wide Configuration Options
# ------------------------------------------------------------------------------

include(EnumOption)

enum_option(RS_BACKEND "raylib;glfw_opengl;sfml;sdl2" "Windowing/graphics backend to use.")

option(RS_BUILD_EXAMPLES     "Build example applications"           ${PROJECT_IS_TOP_LEVEL})
option(RS_BUILD_SHARED       "Build Raysim as a shared library"     ${BUILD_SHARED_LIBS})

option(RS_ENABLE_COVERAGE    "Enable coverage reporting"            OFF)
option(RS_WARNINGS_AS_ERRORS "Treat compiler warnings as errors"    OFF)
option(RS_ENABLE_LTO         "Enable Link Time Optimization"        OFF)
option(RS_ENABLE_PROFILE     "Enable profiling instrumentation"     OFF)

option(RS_SANITIZE_ASAN  "Enable AddressSanitizer (ASAN) for debugging"            OFF)
option(RS_SANITIZE_UBSAN "Enable UndefinedBehaviorSanitizer (UBSan) for debugging" OFF)
option(RS_SANITIZE_TSAN  "Enable ThreadSanitizer (TSan) for debugging"             OFF)
option(RS_SANITIZE_MSAN  "Enable MemorySanitizer (MSan, Clang only)"               OFF)
option(RS_SANITIZE_LSAN  "Enable LeakSanitizer (LSan) for debugging"               OFF)

option(RS_SYSTEM_RAYLIB "Use system-installed raylib instead of building from source" OFF)
option(RS_SYSTEM_GLFW   "Use system-installed GLFW instead of building from source" OFF)

# ------------------------------------------------------------------------------
#  Sanitizer consistency
# ------------------------------------------------------------------------------

# MSan requires Clang.
if(RS_SANITIZE_MSAN AND NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  message(FATAL_ERROR
    "[rs] RS_SANITIZE_MSAN=ON requires Clang "
    "(current compiler: ${CMAKE_CXX_COMPILER_ID})."
  )
endif()

# ASan and TSan are mutually exclusive.
if(RS_SANITIZE_ASAN AND RS_SANITIZE_TSAN)
  message(FATAL_ERROR
    "[rs] RS_SANITIZE_ASAN and RS_SANITIZE_TSAN cannot be enabled simultaneously.")
endif()

# MSan cannot be combined with any other sanitizer.
if(RS_SANITIZE_MSAN AND (RS_SANITIZE_ASAN OR RS_SANITIZE_UBSAN OR RS_SANITIZE_TSAN OR RS_SANITIZE_LSAN))
  message(FATAL_ERROR
    "[rs] RS_SANITIZE_MSAN cannot be combined with any other sanitizer.")
endif()

# LTO is incompatible with sanitizers and with coverage instrumentation.
set(_any_sanitizer OFF)
foreach(_san IN ITEMS ASAN UBSAN TSAN MSAN LSAN)
  if(RS_SANITIZE_${_san})
    set(_any_sanitizer ON)
    break()
  endif()
endforeach()

if(RS_ENABLE_LTO AND (_any_sanitizer OR RS_ENABLE_COVERAGE))
  message(WARNING
    "[rs] RS_ENABLE_LTO disabled: incompatible with sanitizers and coverage instrumentation.")
  set(RS_ENABLE_LTO OFF CACHE BOOL "" FORCE)
endif()

unset(_any_sanitizer)

# ------------------------------------------------------------------------------
#  Shared library configuration
# ------------------------------------------------------------------------------

if(RS_BUILD_SHARED)
  # Embed a relative runtime path so executables can locate libRaysim
  # beside themselves without requiring installation or PATH changes.
  #
  # Linux:   $ORIGIN  -> resolved to the executable's directory at load time.
  # macOS:   @loader_path -> directory of the loading binary.
  # Windows: no RPATH concept; the loader searches the executable directory.
  if(APPLE)
    set(CMAKE_INSTALL_RPATH "@loader_path")
    set(CMAKE_BUILD_RPATH "@loader_path")
  elseif(UNIX)
    set(CMAKE_INSTALL_RPATH "\$ORIGIN")
    set(CMAKE_BUILD_RPATH "\$ORIGIN")
  endif()

  # Static libraries linked into the shared library must be compiled with PIC.
  if(NOT MSVC)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)

    # Export only symbols explicitly marked with RS_API.
    set(CMAKE_CXX_VISIBILITY_PRESET  hidden)
    set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)
  endif()
endif()

# ------------------------------------------------------------------------------
#  Output Layout
# ------------------------------------------------------------------------------

if(PROJECT_IS_TOP_LEVEL)
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

  # Multi-config generators (Visual Studio, Xcode, Ninja Multi-Config) append
  # the active configuration name to the output directories.
  if(CMAKE_CONFIGURATION_TYPES)
    set(RS_OUTPUT_CONFIG_DIR "$<CONFIG>")
  else()
    set(RS_OUTPUT_CONFIG_DIR "")
  endif()

  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${RS_OUTPUT_CONFIG_DIR})
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}/${RS_OUTPUT_CONFIG_DIR})
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}/${RS_OUTPUT_CONFIG_DIR})

  # Default to Debug for single-config generators when no build type is given.
  if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
      "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
  endif()
endif()

# ------------------------------------------------------------------------------
#  Generated Configuration / Versioning Headers
# ------------------------------------------------------------------------------
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/include/Raysim/Version.hpp.in"
  "${PROJECT_BINARY_DIR}/include/Raysim/Version.hpp"
  @ONLY
)

# ------------------------------------------------------------------------------
#  Global Core Modules & System Paths
# ------------------------------------------------------------------------------

include(PlatformDetection)   # RS_PLATFORM_* variables and preprocessor defines
include(Helpers)             # Utility functions for third-party setup
include(Warnings)            # Compiler warning flags
include(CompilerOptimization)
include(CompilerOptions)
include(Sanitizers)          # Runtime sanitizer support
include(Coverage)            # Code coverage instrumentation
include(LTO)                 # Link Time Optimization

# ============================================================================
# DEPENDENCIES
# ============================================================================

include(ThirdParty)

# ============================================================================
# SOURCES
# ============================================================================

add_subdirectory(src)

# ============================================================================
# EXAMPLES
# ============================================================================

if(RS_BUILD_EXAMPLES)
  message(STATUS "[rs] Building examples.")
  add_subdirectory(examples)
endif()
