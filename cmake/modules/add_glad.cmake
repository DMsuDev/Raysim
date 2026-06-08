# ===========================================================================
# Builds the GLAD OpenGL loader from source as a static library.
#
# Expects the submodule at: third_party/glad/
#   third_party/glad/src/gl.c
#   third_party/glad/include/
#
# Exposes:
#   glad        - static library target
#   glad::glad  - namespaced alias
# ===========================================================================

include_guard()

# ---------------------------------------------------------------------------
# Locate submodule
# ---------------------------------------------------------------------------

set(GLAD_DIR "${CMAKE_SOURCE_DIR}/third_party/glad")

foreach(_path IN ITEMS
    "${GLAD_DIR}/src/gl.c"
    "${GLAD_DIR}/include"
)
  if(NOT EXISTS "${_path}")
    message(FATAL_ERROR
      "[rs] add_glad: required path not found: '${_path}'.\n"
      "Run: git submodule update --init --recursive")
  endif()
endforeach()

# ---------------------------------------------------------------------------
# Static library
# ---------------------------------------------------------------------------

add_library(glad STATIC
  "${GLAD_DIR}/src/gl.c"
)

add_library(glad::glad ALIAS glad)

# Force C so the TU is never compiled as C++ (avoids name-mangling issues
# when the parent project sets CMAKE_CXX_STANDARD globally).
set_source_files_properties("${GLAD_DIR}/src/gl.c"
  PROPERTIES LANGUAGE C
)

target_include_directories(glad SYSTEM PUBLIC
  "${GLAD_DIR}/include"
)

# ---------------------------------------------------------------------------
# Engine-wide third-party setup (warnings, compile options, etc.)
# ---------------------------------------------------------------------------

rs_third_party_setup(glad)

# ---------------------------------------------------------------------------
# IDE folder grouping
# ---------------------------------------------------------------------------

set_target_properties(glad PROPERTIES
  FOLDER "ThirdParty/GLAD"
)

message(STATUS "[rs] GLAD configured (${GLAD_DIR})")
