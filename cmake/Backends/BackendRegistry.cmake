# ===========================================================================
# Backend Registry
# Loads the cmake file for the selected backend.
#
# Validation of RS_BACKEND happens earlier, in ProjectOptions.cmake.
# To add a new backend: create cmake/Backends/<name>.cmake.
# ===========================================================================

include_guard()

# ===========================================================================
# Load backend implementation
# ===========================================================================

set(_backend_file "${CMAKE_CURRENT_LIST_DIR}/${RS_BACKEND}.cmake")

if(NOT EXISTS "${_backend_file}")
    message(FATAL_ERROR
        "Backend file not found: ${_backend_file}\n"
        "Create cmake/Backends/${RS_BACKEND}.cmake to add support for this backend.")
endif()

include("${_backend_file}")

message(STATUS "Backend loaded: ${RS_BACKEND}")
