# ============================================================================
#  RAYSIM - BACKEND: GLFW + OPENGL
# ============================================================================
#  Description: GLFW for windowing and input, OpenGL for rendering, GLAD for
#               OpenGL function pointer loading.
#
#  Creates:
#    rs::windowing      - INTERFACE: links glfw + OpenGL::GL
#    rs::graphics       - INTERFACE: links glad + OpenGL::GL
#    rs::imgui_backend  - ALIAS for imgui_backend_glfw_opengl (STATIC)
#
#  Copyright (c) 2026 Dayron Mustelier (@DMsuDev)
#  Licensed under the Apache License, Version 2.0.
# ============================================================================

include_guard()

# ----------------------------------------------------------------------------
# Dependencies
# ----------------------------------------------------------------------------

find_package(glfw3 CONFIG REQUIRED)
find_package(OpenGL       REQUIRED)

# GLAD (built from third_party/)
include(add_glad)

# ----------------------------------------------------------------------------
# rs::windowing  (GLFW + OpenGL)
# ----------------------------------------------------------------------------

add_library(rs_windowing_glfw_opengl INTERFACE)
add_library(rs::windowing ALIAS rs_windowing_glfw_opengl)

target_link_libraries(rs_windowing_glfw_opengl INTERFACE
  glfw
  OpenGL::GL
)

set_target_properties(rs_windowing_glfw_opengl PROPERTIES
  FOLDER "Backends/Windowing"
)

# ----------------------------------------------------------------------------
# rs::graphics  (GLAD + OpenGL)
# ----------------------------------------------------------------------------

add_library(rs_graphics_opengl INTERFACE)
add_library(rs::graphics ALIAS rs_graphics_opengl)

target_link_libraries(rs_graphics_opengl INTERFACE
  glad
  OpenGL::GL
)

set_target_properties(rs_graphics_opengl PROPERTIES
  FOLDER "Backends/Graphics"
)

# ----------------------------------------------------------------------------
# ImGui Backend (GLFW + OpenGL3 Binding)
# ----------------------------------------------------------------------------

include(imgui_backends/imgui_glfw)
