# ===========================================================================
# Backend: glfw_opengl
# GLFW for windowing/input, OpenGL for rendering, GLAD for GL loading.
#
# Required vcpkg packages: glfw3, OpenGL (system), glad (third_party)
# ===========================================================================

include_guard()

# ===========================================================================
# Dependencies
# ===========================================================================

find_package(glfw3 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)

# GLAD - OpenGL loader (solo necesario para OpenGL backends)
include(${CMAKE_SOURCE_DIR}/cmake/modules/add_glad.cmake)

# ===========================================================================
# Windowing (GLFW)
# ===========================================================================

add_library(rs_windowing_glfw_opengl INTERFACE)

target_link_libraries(rs_windowing_glfw_opengl INTERFACE
    glfw
    OpenGL::GL
)

# ===========================================================================
# Graphics (OpenGL + GLAD)
# ===========================================================================

add_library(rs_graphics_opengl INTERFACE)

target_link_libraries(rs_graphics_opengl INTERFACE
    glad
    OpenGL::GL
)

# ===========================================================================
# ImGui backend
# ===========================================================================

include(${CMAKE_SOURCE_DIR}/cmake/imgui_backends/imgui_glfw.cmake)

# ===========================================================================
# Meta-target: rs_backend (combines everything)
# ===========================================================================

add_library(rs_backend INTERFACE)

target_link_libraries(rs_backend INTERFACE
    rs_windowing_glfw_opengl
    rs_graphics_opengl
    ${RS_IMGUI_BACKEND_TARGET}
)

# Grouping for IDEs
set_target_properties(rs_backend PROPERTIES FOLDER "Backends")
set_target_properties(rs_windowing_glfw_opengl PROPERTIES FOLDER "Backends/Windowing")
set_target_properties(rs_graphics_opengl PROPERTIES FOLDER "Backends/Graphics")

message(STATUS "Backend glfw_opengl configured")
