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
  VERSION 0.94.2
  LANGUAGES CXX C
  DESCRIPTION "C++ 2D graphics & simulation framework built on raylib."
)
message(STATUS "[rs] Raysim version: ${PROJECT_VERSION}")

# Disable automatic C++ module dependency scanning; Raysim does not use modules.
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

# Standard install directories (bin, lib, include, etc.) with platform-specific defaults.
include(GNUInstallDirs)

# ------------------------------------------------------------------------------
#  Project-Wide Configuration Options
# ------------------------------------------------------------------------------
option(RS_ENABLE_SANITIZERS  "Enable all sanitizers (ASan + UBSan)" OFF)
option(RS_ENABLE_COVERAGE    "Enable coverage reporting"            OFF)
option(RS_WARNINGS_AS_ERRORS "Treat compiler warnings as errors"    OFF)
option(RS_ENABLE_LTO         "Enable Link Time Optimization"        OFF)
option(RS_ENABLE_PCH         "Enable precompiled headers"           ON)
option(RS_BUILD_EXAMPLES     "Build example applications"           ON)
option(RS_BUILD_SHARED       "Build Raysim as a shared library"     OFF)
option(RS_ENABLE_PROFILE     "Enable profiling instrumentation"     OFF)

# Individual sanitizers: can also be activated via RS_ENABLE_SANITIZERS.
option(RS_SANITIZE_ASAN  "Enable AddressSanitizer"              OFF)
option(RS_SANITIZE_UBSAN "Enable UndefinedBehaviorSanitizer"    OFF)
option(RS_SANITIZE_TSAN  "Enable ThreadSanitizer"               OFF)
option(RS_SANITIZE_MSAN  "Enable MemorySanitizer (Clang only)"  OFF)
option(RS_SANITIZE_LSAN  "Enable LeakSanitizer"                 OFF)

option(RS_SYSTEM_RAYLIB "Use system-installed raylib instead of building from source" OFF)
option(RS_SYSTEM_GLFW   "Use system-installed GLFW instead of building from source" OFF)

# ------------------------------------------------------------------------------
#  Sanitizer consistency
# ------------------------------------------------------------------------------

# RS_ENABLE_SANITIZERS enables ASan + UBSan as a convenience.
if(RS_ENABLE_SANITIZERS)
  set(RS_SANITIZE_ASAN  ON CACHE BOOL "" FORCE)
  set(RS_SANITIZE_UBSAN ON CACHE BOOL "" FORCE)
endif()

# Keep the master flag in sync with individual sanitizers.
if(RS_SANITIZE_ASAN OR RS_SANITIZE_UBSAN OR RS_SANITIZE_TSAN OR
   RS_SANITIZE_MSAN OR RS_SANITIZE_LSAN)
  set(RS_ENABLE_SANITIZERS ON CACHE BOOL "" FORCE)
endif()

# MSan requires Clang.
if(RS_SANITIZE_MSAN AND NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  message(FATAL_ERROR
    "[rs] RS_SANITIZE_MSAN=ON requires Clang "
    "(current compiler: ${CMAKE_CXX_COMPILER_ID})."
  )
endif()

# ASan and TSan cannot be enabled together.
if(RS_SANITIZE_ASAN AND RS_SANITIZE_TSAN)
  message(FATAL_ERROR
    "[rs] RS_SANITIZE_ASAN and RS_SANITIZE_TSAN "
    "cannot be enabled simultaneously."
  )
endif()

# LTO is incompatible with sanitizers and coverage.
if(RS_ENABLE_LTO AND (RS_ENABLE_SANITIZERS OR RS_ENABLE_COVERAGE))
  message(WARNING
    "[rs] RS_ENABLE_LTO disabled because sanitizers or coverage are enabled."
  )
  set(RS_ENABLE_LTO OFF CACHE BOOL "" FORCE)
endif()

# ------------------------------------------------------------------------------
#  Summary of configuration options
# ------------------------------------------------------------------------------

if(PROJECT_IS_TOP_LEVEL)
  message(STATUS "[rs] -----------------------------------------------")
  message(STATUS "[rs] Configuration summary")
  message(STATUS "[rs] -----------------------------------------------")
  message(STATUS "[rs]   Backend          : ${RS_BACKEND}")
  message(STATUS "[rs]   Library type     : ${RS_BUILD_SHARED}")
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

# ------------------------------------------------------------------------------
#  Shared library configuration
# ------------------------------------------------------------------------------

if(RS_BUILD_SHARED)
  # Embed a relative runtime path so executables can locate libRaysim
  # beside themselves after moving the whole folder anywhere on disk.
  #
  # Linux:
  #   $ORIGIN -> executable directory (at runtime, the loader replaces $ORIGIN with the actual path of the executable)
  #
  # macOS:
  #   @loader_path -> directory of the loading binary (executable or dylib)
  #
  # Windows:
  #   No RPATH concept; the loader searches the executable directory and PATH environment variable for DLLs
  #
  # Note: CMAKE_BUILD_RPATH is used during development (e.g. when running from the build directory)
  # to ensure the built executables can find the shared library without needing to install it first.
  if(APPLE)
    set(CMAKE_INSTALL_RPATH "@loader_path")
    set(CMAKE_BUILD_RPATH "@loader_path")
  elseif(UNIX)
    set(CMAKE_INSTALL_RPATH "\$ORIGIN")
    set(CMAKE_BUILD_RPATH "\$ORIGIN")
  endif()

  # Static libraries linked into the shared library must use PIC.
  if(NOT MSVC)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)

    # Export only symbols explicitly marked with RS_API.
    set(CMAKE_CXX_VISIBILITY_PRESET hidden)
    set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)
  endif()

endif()

# ------------------------------------------------------------------------------
#  Isolated Target Architecture & Output Layout
# ------------------------------------------------------------------------------

if(PROJECT_IS_TOP_LEVEL)
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

  # Multi-config generators (Visual Studio, Xcode, Ninja Multi-Config)
  if(CMAKE_CONFIGURATION_TYPES)
    set(RS_OUTPUT_CONFIG_DIR "$<CONFIG>")
  else()
    set(RS_OUTPUT_CONFIG_DIR "")
  endif()

  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${RS_OUTPUT_CONFIG_DIR})
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}/${RS_OUTPUT_CONFIG_DIR})
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}/${RS_OUTPUT_CONFIG_DIR})

  # Fallback to standard Debug profile if none is specified for single-config generators
  if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
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

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Set platform-specific variables and options
include(PlatformDetection)

# Helper functions (for third-party setup)
include(Helpers)

# Set compiler warning flags and options
include(Warnings)
include(CompilerOptimization)
include(CompilerOptions)

# Sanitizers for runtime error detection
include(Sanitizers)

# Code coverage configuration
include(Coverage)

# Link Time Optimization (LTO) configuration
include(LTO)

# ============================================================================
# DEPENDENCIES
# ============================================================================

# Include ThirdParty dependencies
include(ThirdParty)

# ============================================================================
# SOURCES
# ============================================================================

# Configure the library with source files from src/
add_subdirectory(src)

# ============================================================================
# EXAMPLES
# ============================================================================

if(RS_BUILD_EXAMPLES)
  message(STATUS "Building examples is enabled")
  add_subdirectory(examples)
endif()
