# ============================================================================
#  RAYSIM - IMGUI BACKEND: RAYLIB (rlImGui)
# ============================================================================
#  Description: Compiles rlImGui as the glue layer between Raylib and Dear
#               ImGui. Exposes rs::imgui_backend as the canonical alias so
#               BackendRegistry can link it without knowing the concrete name.
#
#  Requires (set by add_imgui.cmake before this file is included):
#    IMGUI_DIR          - root of the ImGui source tree
#
#  Requires (set by raylib.cmake before this file is included):
#    raylib             - CMake target
#
#  Copyright (c) 2026 Dayron Mustelier (@DMsuDev)
#  Licensed under the Apache License, Version 2.0.
# ============================================================================

include_guard()

# ----------------------------------------------------------------------------
# Precondition: add_imgui.cmake must have run first
# ----------------------------------------------------------------------------

if(NOT DEFINED IMGUI_DIR OR "${IMGUI_DIR}" STREQUAL "")
  message(FATAL_ERROR
    "[rs] imgui_raylib.cmake: 'IMGUI_DIR' is not set.\n"
    "     Include add_imgui.cmake (via ThirdParty.cmake) before loading any backend."
  )
endif()

# ----------------------------------------------------------------------------
# Precondition: raylib must already be a target
# ----------------------------------------------------------------------------

if(NOT TARGET raylib)
  message(FATAL_ERROR
    "[rs] imgui_raylib.cmake: target 'raylib' does not exist.\n"
    "     raylib.cmake must include() this file after its find_package/FetchContent call."
  )
endif()

# ----------------------------------------------------------------------------
# rlImGui source
# ----------------------------------------------------------------------------

set(_rlimgui_dir "${PROJECT_SOURCE_DIR}/third_party/rlImGui")

if(NOT EXISTS "${_rlimgui_dir}/rlImGui.cpp")
  message(FATAL_ERROR
    "[rs] imgui_raylib.cmake: rlImGui source not found at '${_rlimgui_dir}'.\n"
    "     Run: git submodule update --init --recursive"
  )
endif()

# ----------------------------------------------------------------------------
# Static library: imgui_backend_raylib
# ----------------------------------------------------------------------------

add_library(imgui_backend_raylib STATIC
  "${_rlimgui_dir}/rlImGui.cpp"
)

add_library(rs::imgui_backend ALIAS imgui_backend_raylib)

target_include_directories(imgui_backend_raylib SYSTEM PUBLIC
  "${_rlimgui_dir}"
  "${IMGUI_DIR}"
)

target_link_libraries(imgui_backend_raylib
  PUBLIC imgui raylib
)

# ----------------------------------------------------------------------------
# Engine-wide third-party setup (silence warnings, etc.)
# ----------------------------------------------------------------------------

rs_third_party_setup(imgui_backend_raylib)

set_target_properties(imgui_backend_raylib PROPERTIES
  FOLDER "ThirdParty/ImGui/Backends"
)

message(STATUS "[rs] ImGui backend loaded: Raylib (rlImGui)")
