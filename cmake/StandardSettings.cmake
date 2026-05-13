# ================================================
# Global project defaults
# ================================================

include_guard()

# Use GNUInstallDirs for standard install directory variables like CMAKE_INSTALL_BINDIR, CMAKE_INSTALL_LIBDIR, etc.
# This allows users to customize install paths in a platform-appropriate way, e.g.:
#   cmake -DCMAKE_INSTALL_PREFIX=/custom/path ..
#
# For more info, see:
# https://cmake.org/cmake/help/latest/module/GNUInstallDirs.html
include(GNUInstallDirs)

# ================================================
# Output directories
# ================================================

if(CMAKE_CONFIGURATION_TYPES)
  # Multi-config generators
  set(RS_OUTPUT_CONFIG_DIR "$<CONFIG>")
else()
  # Single-config generators
  set(RS_OUTPUT_CONFIG_DIR "")
endif()

# Multi-config generators (Visual Studio, Xcode, Ninja Multi-Config)
# should not mix Debug/Release outputs in the same directory.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${RS_OUTPUT_CONFIG_DIR}) # Executables and shared libraries (.exe, .dll, .so, .dylib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}/${RS_OUTPUT_CONFIG_DIR}) # Shared libraries on non-DLL platforms (.so, .dylib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}/${RS_OUTPUT_CONFIG_DIR}) # Static libraries (.a, .lib)

# ================================================
# C++ standard
# ================================================

# Set C++ standard to C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ================================================
# Compile commands database
# ================================================

# Generate compile_commands.json to work with tools like:
# - clangd
# - clang-tidy
# - CodeQL
# - static analysis tools
# - IDE integrations

if(PROJECT_IS_TOP_LEVEL)
  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

# ================================================
# Default build type
# ================================================

# Only applies to single-config generators such as:
# - Ninja
# - Unix Makefiles

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)

  message(STATUS "Setting build type to 'Debug' as none was specified.")

  set(CMAKE_BUILD_TYPE
    Debug
    CACHE STRING "Build type")

  # Set the possible values of build type for cmake-gui, ccmake
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug"
    "Release"
    "MinSizeRel"
    "RelWithDebInfo"
  )

endif()

# Set a suffix for debug builds on multi-config generators (e.g. MyLibd.dll) to avoid conflicts with release builds.
if(CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_DEBUG_POSTFIX d)
endif()

# ================================================
# Disable automatic C++20 module scanning
# ================================================

# Raysim currently does not use C++20 named modules or module partitions.
# Leaving scanning enabled may introduce unnecessary preprocessing passes,
# dependency analysis and compiler-specific module flags, increasing build
# times and memory usage on some toolchains.
#
# More info:
# https://cmake.org/cmake/help/latest/manual/cmake-cxxmodules.7.html
if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.28")
  set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
endif()

# ================================================
# Shared library configuration
# ================================================

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

  # Static libraries linked into shared libraries must be compiled as PIC.
  # MSVC/PE does not require an equivalent to -fPIC.
  if(NOT MSVC)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
  endif()

  # Hide symbols by default to:
  # - reduce exported symbol count
  # - improve link times
  # - reduce binary size
  # - avoid symbol collisions
  #
  # Public API symbols should be explicitly exported using RS_API.

  # For now is disabled cause is not yet implemented in the codebase.
  # if(not MSVC)
  #   set(CMAKE_CXX_VISIBILITY_PRESET hidden)
  #   set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)
  # endif()

endif()

# ================================================
# Grouping in IDEs
# ================================================

# Organize targets into folders in IDEs that support it (e.g. Visual Studio, Xcode).
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
