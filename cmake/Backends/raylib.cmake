# ============================================================================
#  RAYSIM - BACKEND: RAYLIB
# ============================================================================
#  Description: All-in-one backend. Raylib handles windowing, input, and
#               OpenGL rendering internally — no separate graphics target.
#
#  Creates:
#    rs::windowing      - INTERFACE: links raylib
#    rs::graphics       - INTERFACE: delegates to rs::windowing
#    rs::imgui_backend  - ALIAS for imgui_backend_raylib (STATIC, via rlImGui)
#
#  Copyright (c) 2026 Dayron Mustelier (@DMsuDev)
#  Licensed under the Apache License, Version 2.0.
# ============================================================================

include_guard()

if(NOT TARGET raylib)
  message(FATAL_ERROR
    "[Raysim] Backend 'raylib' requires target raylib.\n"
    "Include deps/raylib before including this backend module.")
endif()

# ----------------------------------------------------------------------------
# rs::windowing (Raylib)
# ----------------------------------------------------------------------------

add_library(rs_windowing_raylib INTERFACE)
add_library(rs::windowing ALIAS rs_windowing_raylib)

target_link_libraries(rs_windowing_raylib INTERFACE raylib)

set_target_properties(rs_windowing_raylib PROPERTIES
  FOLDER "Backends/Windowing"
)

# ----------------------------------------------------------------------------
# rs::graphics  (Raylib owns OpenGL — no separate graphics library)
# ----------------------------------------------------------------------------

add_library(rs_graphics_raylib INTERFACE)
add_library(rs::graphics ALIAS rs_graphics_raylib)

# Raylib is monolithic: windowing and graphics are the same library.
target_link_libraries(rs_graphics_raylib INTERFACE rs_windowing_raylib)

set_target_properties(rs_graphics_raylib PROPERTIES
  FOLDER "Backends/Graphics"
)

# ----------------------------------------------------------------------------
#  ImGui Backend (rlImGui Binding)
# ----------------------------------------------------------------------------

include(imgui_backends/imgui_raylib)
