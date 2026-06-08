# ===========================================================================
# Backend: raylib
# Raylib is "all-in-one": windowing + input + rendering in a single lib.
#
# Required vcpkg packages: raylib (glfw3 pulled in automatically when
#                          USE_EXTERNAL_GLFW=ON is set by vcpkg)
# ===========================================================================

include_guard()

# ===========================================================================
# Dependencies
# ===========================================================================

find_package(raylib CONFIG REQUIRED)

# vcpkg builds raylib with USE_EXTERNAL_GLFW=ON, so GLFW symbols are NOT
# baked into libraylib.a and must be linked explicitly to avoid undefined
# symbol errors at link time.
find_package(glfw3 CONFIG QUIET)

# ===========================================================================
# Windowing target  (Raylib + optional explicit GLFW)
# ===========================================================================

add_library(rs_windowing_raylib INTERFACE)
add_library(rs::windowing       ALIAS rs_windowing_raylib)

if(glfw3_FOUND)
  # LINK_GROUP:RESCAN emits --start-group/--end-group, which is only
  # supported by GNU-style linkers (GCC/Clang on Linux and macOS).
  # On Windows we link both libraries directly to stay MSVC-compatible.
  if(WIN32)
    target_link_libraries(rs_windowing_raylib INTERFACE raylib glfw)
  else()
    target_link_libraries(rs_windowing_raylib INTERFACE
      "$<LINK_GROUP:RESCAN,raylib,glfw>"
    )
  endif()
else()
  # Fallback: assume GLFW is baked into the raylib static archive.
  target_link_libraries(rs_windowing_raylib INTERFACE raylib)
endif()

# ===========================================================================
# Graphics target  (Raylib manages OpenGL internally - no GLAD needed)
# ===========================================================================

add_library(rs_graphics_raylib INTERFACE)
add_library(rs::graphics       ALIAS rs_graphics_raylib)

# Graphics intentionally reuses the windowing target: Raylib bundles both
# concerns, so there is no separate graphics library to link against.
target_link_libraries(rs_graphics_raylib INTERFACE
  rs_windowing_raylib
)

# ===========================================================================
# ImGui backend  (rlImGui binding)
# ===========================================================================

include("${CMAKE_SOURCE_DIR}/cmake/imgui_backends/imgui_raylib.cmake")

# ---------------------------------------------------------------------------
# IDE folder grouping
# ---------------------------------------------------------------------------

set_target_properties(rs_windowing_raylib PROPERTIES FOLDER "Backends/Windowing")
set_target_properties(rs_graphics_raylib  PROPERTIES FOLDER "Backends/Graphics")

message(STATUS "[rs] Backend configured: raylib")
