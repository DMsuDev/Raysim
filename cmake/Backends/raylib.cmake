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

# vcpkg builds raylib with USE_EXTERNAL_GLFW=ON. CMake places GLFW before
# raylib in the link command, which breaks single-pass linkers (debug/no-LTO).
# LINK_GROUP:RESCAN wraps both in --start-group/--end-group to allow
# multiple passes.
find_package(glfw3 CONFIG QUIET)

# ===========================================================================
# Windowing (Raylib handles everything internally)
# ===========================================================================

add_library(rs_windowing_raylib INTERFACE)

if(glfw3_FOUND)
    # LINK_GROUP:RESCAN uses --start-group/--end-group, only supported by GNU-style linkers (Linux/macOS).
    # On Windows (MSVC) link both libraries directly without the group wrapper.
    if(MSVC)
        target_link_libraries(rs_windowing_raylib INTERFACE raylib glfw)
    else()
        target_link_libraries(rs_windowing_raylib INTERFACE
            "$<LINK_GROUP:RESCAN,raylib,glfw>"
        )
    endif()
else()
    target_link_libraries(rs_windowing_raylib INTERFACE raylib)
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
