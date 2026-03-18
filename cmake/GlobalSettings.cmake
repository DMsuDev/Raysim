# ============================================================================
# OUTPUT DIRECTORIES AND PATHS
# ============================================================================

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

# Generate compile_commands.json for tools like clangd, ccls, etc.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL "Generate compile_commands.json" FORCE)

# ============================================================================
# C++ STANDARD
# ============================================================================

set(CMAKE_CXX_STANDARD           20)
set(CMAKE_CXX_STANDARD_REQUIRED  ON)
set(CMAKE_CXX_EXTENSIONS         OFF)

# ============================================================================
# BUILD OPTIONS & CONFIGURATION
# ============================================================================

set(THREADS_PREFER_PTHREAD_FLAG ON CACHE BOOL "" FORCE)
set(CMAKE_POSITION_INDEPENDENT_CODE ON CACHE BOOL "" FORCE)  # -fPIC by default

# Enable multi-processor compilation in MSVC (faster builds)
if(MSVC)
    add_compile_options(/MP)
endif()
