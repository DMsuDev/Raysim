# ============================================================================
#  RAYSIM - ADD GLAD
# ============================================================================
#  Description: Adds the glad third-party library as a STATIC target.
#
#  Creates:
#    glad        - STATIC library
#    glad::glad  - namespaced alias
#
#  Copyright (c) 2026 Dayron Mustelier (@DMsuDev)
#  Licensed under the Apache License, Version 2.0.
# ============================================================================

include_guard()

# ---------------------------------------------------------------------------
# Paths
# ---------------------------------------------------------------------------

set(GLAD_DIR "${PROJECT_SOURCE_DIR}/third_party/glad")

# Guard against multiple inclusion of this module (for future-proofing)
if(TARGET glad)
  message(STATUS "[rs] glad target already exists — skipping configuration.")
  return()
endif()

# ---------------------------------------------------------------------------
# Target: glad (STATIC)
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

# ----------------------------------------------------------------------------
# Engine-wide third-party setup (silence warnings, etc.)
# ----------------------------------------------------------------------------

rs_third_party_setup(glad)

set_target_properties(glad PROPERTIES
  FOLDER "ThirdParty/GLAD"
)

message(STATUS "[rs] GLAD configured (${GLAD_DIR})")
