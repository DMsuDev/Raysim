# ================================================
# Global project defaults
# ================================================

include_guard()

# Set output directories for binaries and libraries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Set C++ standard to C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Debug' as none was specified.")
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui, ccmake
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
             "Debug"
             "Release"
             "MinSizeRel"
             "RelWithDebInfo")
endif()

# Generate compile_commands.json to make it easier to work with clang based tools
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Enable Link Time Optimization if requested
if(RS_ENABLE_LTO)
  include(CheckIPOSupported)
  check_ipo_supported(RESULT result OUTPUT error)

  if(result)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
  else()
    message(WARNING "IPO not supported: ${error}")
  endif()
endif()
