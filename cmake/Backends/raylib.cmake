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
# We wrap both in --start-group/--end-group on GNU-style linkers to allow
# multiple resolution passes.
find_package(glfw3 CONFIG QUIET)

# ===========================================================================
# Windowing (Raylib handles everything internally)
# ===========================================================================

add_library(rs_windowing_raylib INTERFACE)

if(glfw3_FOUND)
    # Check if using MSVC linker or lld-link (which don't support --start/end-group)
    string(TOLOWER "${CMAKE_LINKER}" CMAKE_LINKER_LOWER)
    if(MSVC OR CMAKE_LINKER_LOWER MATCHES "lld-link")
        target_link_libraries(rs_windowing_raylib INTERFACE raylib glfw)
    else()
        # Wrap both libs in --start-group/--end-group so single-pass GNU-style
        # linkers (ld, gold) can resolve the circular dependency between
        # raylib and glfw without needing LINK_GROUP:RESCAN.
        # Note: lld-link (LLVM linker on Windows) doesn't support these flags.
        target_link_libraries(rs_windowing_raylib INTERFACE
            -Wl,--start-group
            raylib
            glfw
            -Wl,--end-group
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
