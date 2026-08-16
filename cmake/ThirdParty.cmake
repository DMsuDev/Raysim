# ============================================================================
#  RAYSIM - THIRD-PARTY DEPENDENCY MANAGEMENT
# ============================================================================
#  Description: Single entry point for all third-party dependencies.
#               Strategy per dependency:
#
#  Option:
#    RS_USE_FETCH_CONTENT  [OFF by default]
#      Set ON to skip find_package() and download dependencies automatically.
#      Useful for end-users building without vcpkg.
#
#  Copyright (c) 2026 Dayron Mustelier (@DMsuDev)
#  Licensed under the Apache License, Version 2.0.
# ============================================================================

include_guard()

# ---------------------------------------------------------------------------
# Third-party libraries
# ---------------------------------------------------------------------------

find_package(Threads REQUIRED)

include(add_spdlog)
include(add_stb)
include(add_imgui)

# ---------------------------------------------------------------------------
# Backend (windowing + graphics + ImGui binding)
# ---------------------------------------------------------------------------

include(Backends/BackendRegistry)
