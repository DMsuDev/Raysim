# ===========================================================================
# ThirdParty.cmake
# ===========================================================================

include_guard()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules")

# ---------------------------------------------------------------------------
# System Threads  (required by spdlog)
# ---------------------------------------------------------------------------

find_package(Threads REQUIRED)

# ---------------------------------------------------------------------------
# Third-party libraries
# ---------------------------------------------------------------------------

include(add_spdlog)
include(add_stb)
include(add_imgui)

# ---------------------------------------------------------------------------
# Backend (windowing + graphics + ImGui binding)
# ---------------------------------------------------------------------------

include("${CMAKE_SOURCE_DIR}/cmake/Backends/BackendRegistry.cmake")
