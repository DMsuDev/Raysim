# ===========================================================================
# ImGui backend: Raylib  (via rlImGui)
#
# rlImGui acts as the glue layer between Raylib and Dear ImGui.
# Compiles rlImGui.cpp into a static library and exposes the two
# conventional variables consumed by BackendRegistry:
#
#   RS_IMGUI_BACKEND_TARGET          - CMake target to link against
#   RS_IMGUI_BACKEND_EXTRA_INCLUDES  - additional include dirs for the engine
#
# Required vcpkg packages: raylib  (already resolved by the Raylib backend)
# ===========================================================================

include_guard()

# ---------------------------------------------------------------------------
# Validate upstream variables
# Defined by add_imgui.cmake; fail early with a clear message if missing.
# ---------------------------------------------------------------------------

foreach(_var IN ITEMS IMGUI_DIR IMGUI_BACKENDS_DIR)
  if(NOT DEFINED ${_var} OR "${${_var}}" STREQUAL "")
    message(FATAL_ERROR
      "[rs] imgui_raylib.cmake: '${_var}' is not set. "
      "Make sure add_imgui.cmake is included before this file.")
  endif()
endforeach()

# ---------------------------------------------------------------------------
# rlImGui source directory
# ---------------------------------------------------------------------------

set(RLIMGUI_DIR "${CMAKE_SOURCE_DIR}/third_party/rlImGui")

if(NOT EXISTS "${RLIMGUI_DIR}/rlImGui.cpp")
  message(FATAL_ERROR
    "[rs] rlImGui source not found at '${RLIMGUI_DIR}'. "
    "Run: git submodule update --init --recursive")
endif()

# ---------------------------------------------------------------------------
# Static library: imgui_backend_raylib
# ---------------------------------------------------------------------------

add_library(imgui_backend_raylib STATIC
  "${RLIMGUI_DIR}/rlImGui.cpp"
)

add_library(rs::imgui_backend ALIAS imgui_backend_raylib)

target_include_directories(imgui_backend_raylib SYSTEM PUBLIC
  "${RLIMGUI_DIR}"
  "${IMGUI_DIR}"
)

target_link_libraries(imgui_backend_raylib
  PUBLIC  imgui raylib
)

# ---------------------------------------------------------------------------
# Engine-wide helper (compile options, warnings, folder, etc.)
# ---------------------------------------------------------------------------

rs_third_party_setup(imgui_backend_raylib)

# ---------------------------------------------------------------------------
# IDE folder grouping
# ---------------------------------------------------------------------------

set_target_properties(imgui_backend_raylib PROPERTIES
  FOLDER "ThirdParty/ImGui/Backends"
)

# ---------------------------------------------------------------------------
# Outputs consumed by BackendRegistry
# ---------------------------------------------------------------------------

set(RS_IMGUI_BACKEND_TARGET         imgui_backend_raylib  CACHE INTERNAL "")
set(RS_IMGUI_BACKEND_EXTRA_INCLUDES "${RLIMGUI_DIR}"      CACHE INTERNAL "")

message(STATUS "[rs] ImGui backend loaded: Raylib (rlImGui)")
