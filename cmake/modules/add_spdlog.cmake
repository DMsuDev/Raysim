# ============================================================================
#  RAYSIM - ADD SPDLOG
# ============================================================================
#  Description: Adds the spdlog third-party library as a header-only INTERFACE target.
#
#  Creates:
#    spdlog         - INTERFACE library
#    spdlog::spdlog - namespaced alias
#
#  Copyright (c) 2026 Dayron Mustelier (@DMsuDev)
#  Licensed under the Apache License, Version 2.0.
# ============================================================================

include_guard()

# ---------------------------------------------------------------------------
# Locate
# ---------------------------------------------------------------------------

set(SPDLOG_DIR "${PROJECT_SOURCE_DIR}/third_party/spdlog")

# Guard against multiple inclusion of this module (for future-proofing)
if(TARGET spdlog)
  message(STATUS "[rs] spdlog target already exists — skipping configuration.")
  return()
endif()

# ---------------------------------------------------------------------------
# Interface library
# ---------------------------------------------------------------------------

add_library(spdlog INTERFACE)
add_library(spdlog::spdlog ALIAS spdlog)

target_include_directories(spdlog SYSTEM INTERFACE
  "${SPDLOG_DIR}/include"
)

target_link_libraries(spdlog INTERFACE Threads::Threads)

# ----------------------------------------------------------------------------
# Engine-wide third-party setup (silence warnings, etc.)
# ----------------------------------------------------------------------------

rs_third_party_setup(spdlog)

set_target_properties(spdlog PROPERTIES
  FOLDER "ThirdParty/SPDLOG"
)
