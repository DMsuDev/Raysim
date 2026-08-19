# ============================================================================
#  RAYSIM - BACKEND: GLFW + OPENGL
# ============================================================================
#  Description: GLFW for windowing and input, OpenGL for rendering, GLAD for
#               OpenGL function pointer loading.
#
#  Creates:
#    rs::windowing      -- INTERFACE: links glfw + OpenGL::GL
#    rs::graphics       -- INTERFACE: links glad + OpenGL::GL
#    rs::imgui_backend  -- ALIAS for imgui_backend_glfw_opengl (STATIC)
#
#  Prerequisites (resolved by ThirdParty.cmake before BackendRegistry):
#    glfw        -- from deps/glfw  (bundled submodule or vcpkg)
#    glad        -- from modules/add_glad  (always built from source)
#    OpenGL::GL  -- resolved here directly (always system, never bundled)
#
#  Copyright (c) 2026 Dayron Mustelier (@DMsuDev)
#  Licensed under the Apache License, Version 2.0.
# ============================================================================

include_guard()

# ----------------------------------------------------------------------------
# Prerequisite guards
# ----------------------------------------------------------------------------

if(NOT TARGET glfw)
  message(FATAL_ERROR
    "[rs] Backend 'glfw_opengl' requires target glfw.\n"
    "     Include deps/glfw before BackendRegistry in ThirdParty.cmake.")
endif()

if(NOT TARGET glad)
  message(FATAL_ERROR
    "[rs] Backend 'glfw_opengl' requires target glad.\n"
    "     Include modules/add_glad before BackendRegistry in ThirdParty.cmake.")
endif()

# ----------------------------------------------------------------------------
# Dependencies
# ----------------------------------------------------------------------------

# OpenGL is always a system dependency.
find_package(OpenGL REQUIRED)

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
