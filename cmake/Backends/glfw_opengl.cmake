# ===========================================================================
# Backend: glfw_opengl
# GLFW for windowing/input, OpenGL for rendering, GLAD for GL loading.
#
# Required vcpkg packages: glfw3, glad (third_party), OpenGL (system)
# ===========================================================================

include_guard()

# ===========================================================================
# Dependencies
# ===========================================================================

find_package(glfw3  CONFIG REQUIRED)
find_package(OpenGL        REQUIRED)

# GLAD - OpenGL function loader (built from third_party/)
include("${CMAKE_SOURCE_DIR}/cmake/modules/add_glad.cmake")

# ===========================================================================
# Windowing target (GLFW)
# ===========================================================================

add_library(rs_windowing_glfw_opengl INTERFACE)
add_library(rs::windowing            ALIAS rs_windowing_glfw_opengl)

target_link_libraries(rs_windowing_glfw_opengl INTERFACE
  glfw
  OpenGL::GL
)

# ===========================================================================
# Graphics target (OpenGL + GLAD)
# ===========================================================================

add_library(rs_graphics_opengl INTERFACE)
add_library(rs::graphics       ALIAS rs_graphics_opengl)

target_link_libraries(rs_graphics_opengl INTERFACE
  glad
  OpenGL::GL
)

# ===========================================================================
# ImGui backend (GLFW + OpenGL3 binding)
# ===========================================================================

include("${CMAKE_SOURCE_DIR}/cmake/imgui_backends/imgui_glfw.cmake")

# ---------------------------------------------------------------------------
# IDE folder grouping
# ---------------------------------------------------------------------------

set_target_properties(rs_windowing_glfw_opengl PROPERTIES FOLDER "Backends/Windowing")
set_target_properties(rs_graphics_opengl       PROPERTIES FOLDER "Backends/Graphics")

message(STATUS "[rs] Backend configured: glfw_opengl")
