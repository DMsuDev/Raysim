cmake_minimum_required(VERSION 3.28)
project(Raysim VERSION 0.91.1 LANGUAGES CXX C)

# Module path for easier includes
# Anywhere you see include(...) you can check <root>/cmake for that file
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# ============================================================================
# CONFIGURATION
# ============================================================================

# Set platform-specific variables and options
include(PlatformDetection)

# Include project options (like enabling sanitizers, coverage, etc)
# NOTE: must come before find_package so RS_IMGUI_BACKEND is defined
include(ProjectOptions)

# Standard project settings (C++ standard, build type, etc)
include(StandardSettings)

# Helper functions (for third-party setup)
include(Helpers)

# Compiler configuration
include(compiler/CompilerWarnings)
include(compiler/CompilerOptimization)
include(compiler/CompilerOptions)

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
