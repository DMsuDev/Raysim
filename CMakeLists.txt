cmake_minimum_required(VERSION 3.28)
project(Raysim VERSION 0.2.1 LANGUAGES CXX C)

# ============================================================================
# OUTPUT DIRECTORIES AND PATHS
# ============================================================================

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL "Generate compile_commands.json" FORCE)

# ============================================================================
# C++ STANDARDS & GLOBAL SETTINGS
# ============================================================================

set(CMAKE_CXX_STANDARD          17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)
set(BUILD_SHARED_LIBS           OFF CACHE BOOL "Build shared libraries" FORCE)
set(THREADS_PREFER_PTHREAD_FLAG ON)

# ============================================================================
# BUILD OPTIONS & CONFIGURATION
# ============================================================================

# Treat warnings as errors (useful for maintaining code quality)
set(WARNINGS_AS_ERRORS OFF CACHE BOOL "Treat compiler warnings as errors" FORCE)

# Hide console window on Windows Release builds
set(HIDE_CONSOLE ON CACHE BOOL "Hide console window on Windows in Release builds" FORCE)

# Enable debug assertions and logging
set(ENABLE_DEBUG OFF CACHE BOOL "Enable debug mode with assertions and verbose logging" FORCE)

# Set default build type if not specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type (Debug/Release/RelWithDebInfo)" FORCE)
endif()

# ============================================================================
# COMPILER-SPECIFIC WARNINGS AND FLAGS
# ============================================================================

# Collect warning flags to apply to all targets
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(WARNING_FLAGS
        -Wall              # All common warnings
        -Wextra            # Extra warnings beyond -Wall
        -Wpedantic         # Warnings about non-standard C++
        -Wshadow           # Variable shadowing
        -Wconversion       # Implicit type conversions
        -Wnon-virtual-dtor # Missing virtual destructors
    )
    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        list(APPEND WARNING_FLAGS -Wdocumentation)  # Documentation warnings (Clang only)
    endif()
elseif(MSVC)
    set(WARNING_FLAGS
        /W3                # Warning level 3 (comprehensive)
        /EHsc              # Exception handling: synchronous C++ exceptions
        /permissive-       # Strict standards conformance
        /Zc:__cplusplus    # Correct __cplusplus macro
        /MP                # Multi-processor compilation
    )
endif()

# Optional: Convert warnings to errors (for stricter development)
if(WARNINGS_AS_ERRORS)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        list(APPEND WARNING_FLAGS -Werror)
    elseif(MSVC)
        list(APPEND WARNING_FLAGS /WX)
    endif()
endif()

# ============================================================================
# LIBRARY
# ============================================================================

add_subdirectory(vendor)

# ============================================================================
# PROJECT SOURCE FILES - CREATE LIBRARY
# ============================================================================

set(RAYSIM_SOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Create RaySim as a static library
add_library(${PROJECT_NAME} STATIC
    # Implementation sources
    ${RAYSIM_SOURCES_DIR}/Graphics/Color.cpp
    ${RAYSIM_SOURCES_DIR}/Graphics/Shapes.cpp
    ${RAYSIM_SOURCES_DIR}/Graphics/Texts.cpp

    ${RAYSIM_SOURCES_DIR}/Manager/FontManager.cpp
    ${RAYSIM_SOURCES_DIR}/Manager/InputManager.cpp
    ${RAYSIM_SOURCES_DIR}/Manager/TimeManager.cpp

    ${RAYSIM_SOURCES_DIR}/Core/Canvas.cpp

    ${RAYSIM_SOURCES_DIR}/Utils/Log.cpp

    ${RAYSIM_SOURCES_DIR}/Math/Random.cpp
)

# ============================================================================
# TARGET CONFIGURATION
# ============================================================================

# Include directories for header search
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Link libraries
target_link_libraries(${PROJECT_NAME} PUBLIC
    raylib
    spdlog::spdlog_header_only
)

# Set precompile header (optional, can speed up compilation)
target_precompile_headers(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/pch.hpp
)

# C++ features and warning flags
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
target_compile_options(${PROJECT_NAME} PRIVATE ${WARNING_FLAGS})

# Suppress MSVC security warnings (CRT_SECURE_NO_WARNINGS)
if(MSVC)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

# Enable debug mode: assertions and verbose logging
if(ENABLE_DEBUG)
    target_compile_definitions(${PROJECT_NAME} PRIVATE AG_DEBUG)
    message(STATUS "Debug mode enabled: Assertions and logging will be active")
endif()

# ============================================================================
# EXAMPLES
# ============================================================================

option(RS_BUILD_EXAMPLES "Build Raysim examples" ON)

if(RS_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# ============================================================================
# BUILD SUMMARY
# ============================================================================

message(STATUS "────────────────────────────────────────────────────────────────")
message(STATUS "Project Configuration Summary")
message(STATUS "────────────────────────────────────────────────────────────────")
message(STATUS "Project:            ${PROJECT_NAME} v${PROJECT_VERSION}")
message(STATUS "C++ Standard:       C++${CMAKE_CXX_STANDARD}")
message(STATUS "Compiler:           ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "Build Type:         ${CMAKE_BUILD_TYPE}")
message(STATUS "Generator:          ${CMAKE_GENERATOR}")
message(STATUS "Output Directory:   ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message(STATUS "")
message(STATUS "Build Options:")
message(STATUS "  Warnings as Errors: ${WARNINGS_AS_ERRORS}")
message(STATUS "  Debug Mode:         ${ENABLE_DEBUG}")
message(STATUS "  Hide Console:       ${HIDE_CONSOLE}")
message(STATUS "────────────────────────────────────────────────────────────────")
