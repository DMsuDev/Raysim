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

# vcpkg builds raylib with USE_EXTERNAL_GLFW=ON, so GLFW symbols are not
# baked into libraylib.a, we must link it explicitly.
find_package(glfw3 CONFIG REQUIRED)

# ===========================================================================
# Windowing
# ===========================================================================

add_library(rs_windowing_raylib INTERFACE)

if(APPLE)
    # ld64 (macOS) resolves circular refs automatically; no grouping needed.
    target_link_libraries(rs_windowing_raylib INTERFACE raylib glfw)

elseif(MSVC)
    # lld-link / link.exe do multi-pass resolution by default.
    target_link_libraries(rs_windowing_raylib INTERFACE raylib glfw)

else()
    # GNU ld / gold are single-pass by default; --start-group forces
    # multiple resolution passes to break the raylib <-> glfw cycle.
    target_link_libraries(rs_windowing_raylib INTERFACE
        -Wl,--start-group raylib glfw -Wl,--end-group
    )
endif()

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
