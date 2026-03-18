cmake_minimum_required(VERSION 3.10..3.26)
project(Raysim VERSION 0.3.0 LANGUAGES CXX C)

# ============================================================================
# CONFIGURATION
# ============================================================================

# Directory for easier includes
# Anywhere you see include(...) you can check <root>/cmake for that file
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Global settings: output directories, C++ standard, etc.
include(GlobalSettings)

# User-facing options
include(ProjectOptions)

# Sanitizers (AddressSanitizer, LeakSanitizer, ...)
include(Sanitizers)

# ============================================================================
# DEPENDENCIES
# ============================================================================

add_subdirectory(vendor)

# ============================================================================
# PROJECT LIBRARY
# ============================================================================

# Configure the library with source files from src/
add_subdirectory(src)

# ============================================================================
# EXAMPLES
# ============================================================================

if(RAYSIM_BUILD_EXAMPLES)
    message(STATUS "Building examples is enabled")
    add_subdirectory(examples)
endif()
