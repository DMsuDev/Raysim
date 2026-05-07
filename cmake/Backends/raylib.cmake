# ===========================================================================
# Backend: raylib
# Raylib es "all-in-one": windowing + input + rendering en una lib.
#
# Required vcpkg packages: raylib
# ===========================================================================

include_guard()

# ===========================================================================
# Dependencies
# ===========================================================================

find_package(raylib CONFIG REQUIRED)

# On desktop platforms, vcpkg typically builds raylib with
# USE_EXTERNAL_GLFW=ON.
#
# Some vcpkg/raylib configurations may fail to propagate GLFW
# as a transitive dependency, causing undefined GLFW symbols
# at link time.
#
# Link glfw explicitly to ensure reliable desktop builds.
find_package(glfw3 CONFIG QUIET)

# ===========================================================================
# Windowing (Raylib handles everything internally)
# ===========================================================================

add_library(rs_windowing_raylib INTERFACE)

target_link_libraries(rs_windowing_raylib INTERFACE
    raylib
    $<$<BOOL:${glfw3_FOUND}>:glfw>
)

# ===========================================================================
# Graphics (Raylib handles OpenGL internally - no GLAD needed)
# ===========================================================================

add_library(rs_graphics_raylib INTERFACE)

target_link_libraries(rs_graphics_raylib INTERFACE
    rs_windowing_raylib
)

# ===========================================================================
# ImGui backend
# ===========================================================================

include(${CMAKE_SOURCE_DIR}/cmake/imgui_backends/imgui_raylib.cmake)

# ===========================================================================
# Meta-target: rs_backend (combines everything)
# ===========================================================================

add_library(rs_backend INTERFACE)

target_link_libraries(rs_backend INTERFACE
    rs_windowing_raylib
    rs_graphics_raylib
    ${RS_IMGUI_BACKEND_TARGET}
)

# Grouping for IDEs
set_target_properties(rs_backend PROPERTIES FOLDER "Backends")
set_target_properties(rs_windowing_raylib PROPERTIES FOLDER "Backends/Windowing")
set_target_properties(rs_graphics_raylib PROPERTIES FOLDER "Backends/Graphics")

message(STATUS "Backend raylib configured")
