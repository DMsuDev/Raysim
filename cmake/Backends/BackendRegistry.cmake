# ===========================================================================
# Backend Registry
#
# Resolves and loads the CMake file that corresponds to the backend
# selected via RS_BACKEND (validated earlier in ProjectOptions.cmake).
#
# Convention: each backend lives at cmake/Backends/<name>.cmake.
# To add a new backend, simply create that file, no changes needed here.
# ===========================================================================

include_guard()

# ---------------------------------------------------------------------------
# Resolve backend file
# ---------------------------------------------------------------------------

set(_backend_file "${CMAKE_CURRENT_LIST_DIR}/${RS_BACKEND}.cmake")

if(NOT EXISTS "${_backend_file}")
  message(FATAL_ERROR
    "[rs] BackendRegistry: no implementation file found for backend '${RS_BACKEND}'.\n"
    "Expected path : ${_backend_file}\n"
    "To add support : create cmake/Backends/${RS_BACKEND}.cmake\n"
    "Available backends: ${CMAKE_CURRENT_LIST_DIR}/*.cmake")
endif()

# ---------------------------------------------------------------------------
# Load backend
# ---------------------------------------------------------------------------

include("${_backend_file}")

# ---------------------------------------------------------------------------
# Post-load validation
# Make sure the backend file set the two mandatory output variables.
# ---------------------------------------------------------------------------

foreach(_var IN ITEMS RS_IMGUI_BACKEND_TARGET RS_IMGUI_BACKEND_EXTRA_INCLUDES)
  if(NOT DEFINED ${_var} OR "${${_var}}" STREQUAL "")
    message(FATAL_ERROR
      "[rs] BackendRegistry: backend '${RS_BACKEND}' did not set '${_var}'. "
      "Every backend cmake file must define RS_IMGUI_BACKEND_TARGET "
      "and RS_IMGUI_BACKEND_EXTRA_INCLUDES.")
  endif()
endforeach()

# ---------------------------------------------------------------------------
# Meta-target: rs::backend
# ---------------------------------------------------------------------------

add_library(rs_backend  INTERFACE)
add_library(rs::backend ALIAS rs_backend)

target_link_libraries(rs_backend INTERFACE
  rs::windowing
  rs::graphics
  "${RS_IMGUI_BACKEND_TARGET}"
)

set_target_properties(rs_backend PROPERTIES FOLDER "Backends")

message(STATUS "[rs] Backend registry: loaded '${RS_BACKEND}'")
