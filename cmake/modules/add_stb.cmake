# ===========================================================================
#  RAYSIM - ADD STB
# ===========================================================================
#  Description: Adds the stb third-party library as a header-only INTERFACE target.
#
#  Creates:
#    stb      - INTERFACE library
#    stb::stb - namespaced alias
#
#  Copyright (c) 2026 Dayron Mustelier (@DMsuDev)
#  Licensed under the Apache License, Version 2.0.
# ===========================================================================

include_guard()

# ---------------------------------------------------------------------------
# Locate
# ---------------------------------------------------------------------------

set(STB_DIR "${PROJECT_SOURCE_DIR}/third_party/stb")

# Guard against multiple inclusion of this module (for future-proofing)
if(TARGET stb)
  message(STATUS "[rs] stb target already exists — skipping configuration.")
  return()
endif()

# ---------------------------------------------------------------------------
# Verify required headers exist
# ---------------------------------------------------------------------------

foreach(_header IN ITEMS
  "${STB_DIR}/stb_image.h"
  "${STB_DIR}/stb_image_write.h"
  "${STB_DIR}/stb_image_resize2.h"
)
  if(NOT EXISTS "${_header}")
    message(FATAL_ERROR
      "[rs] add_stb: required header not found: '${_header}'.")
  endif()
endforeach()

# ---------------------------------------------------------------------------
# Interface library
# ---------------------------------------------------------------------------

add_library(stb INTERFACE)
add_library(stb::stb ALIAS stb)

target_include_directories(stb SYSTEM INTERFACE
  "${STB_DIR}"
)

# ---------------------------------------------------------------------------
# Engine-wide third-party setup (silence warnings, IDE folder, etc.)
# ---------------------------------------------------------------------------

rs_third_party_setup(stb)

set_target_properties(stb PROPERTIES
  FOLDER "ThirdParty/STB"
)
