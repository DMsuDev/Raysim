cmake_minimum_required(VERSION 3.28)
project(Raysim VERSION 0.4.0 LANGUAGES CXX C)

# Module path for easier includes
# Anywhere you see include(...) you can check <root>/cmake for that file
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# ============================================================================
# DEPENDENCIES
# ============================================================================

find_package(Threads REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(glfw3  CONFIG REQUIRED)
find_package(raylib CONFIG REQUIRED)
find_package(OpenGL REQUIRED)

# ============================================================================
# CONFIGURATION
# ============================================================================

# Set platform-specific variables and options
include(PlatformDetection)

# Include project options (like enabling sanitizers, coverage, etc)
include(ProjectOptions)

# Standard project settings (C++ standard, build type, etc)
include(StandardSettings)

# Utilities for installation, packaging, etc
include(Utils)

# Compiler warnings configuration
include(CompilerWarnings)

# Sanitizers for runtime error detection
include(Sanitizers)

# Code coverage configuration
include(Coverage)

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
