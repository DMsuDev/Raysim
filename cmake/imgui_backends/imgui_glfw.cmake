# ===========================================================================
# ImGui backend: GLFW + OpenGL3
#
# Compiles imgui_impl_glfw + imgui_impl_opengl3 into a static library and
# exposes the two conventional variables consumed by BackendRegistry:
#
#   RS_IMGUI_BACKEND_TARGET          - CMake target to link against
#   RS_IMGUI_BACKEND_EXTRA_INCLUDES  - additional include dirs for the engine
#
# Required vcpkg packages: glfw3, OpenGL (system), glad (third_party)
# ===========================================================================

include_guard()

# ---------------------------------------------------------------------------
# Validate upstream variables
# Defined by add_imgui.cmake; fail early with a clear message if missing.
# ---------------------------------------------------------------------------

foreach(_var IN ITEMS IMGUI_DIR IMGUI_BACKENDS_DIR)
  if(NOT DEFINED ${_var} OR "${${_var}}" STREQUAL "")
    message(FATAL_ERROR
      "[rs] imgui_glfw.cmake: '${_var}' is not set. "
      "Make sure add_imgui.cmake is included before this file.")
  endif()
endforeach()

# ---------------------------------------------------------------------------
# Static library: imgui_backend_glfw_opengl
# ---------------------------------------------------------------------------

add_library(imgui_backend_glfw_opengl STATIC
  "${IMGUI_BACKENDS_DIR}/imgui_impl_glfw.cpp"
  "${IMGUI_BACKENDS_DIR}/imgui_impl_opengl3.cpp"
)

add_library(rs::imgui_backend ALIAS imgui_backend_glfw_opengl)

target_include_directories(imgui_backend_glfw_opengl SYSTEM PUBLIC
  "${IMGUI_DIR}"
  "${IMGUI_BACKENDS_DIR}"
)

target_link_libraries(imgui_backend_glfw_opengl
  PUBLIC  imgui
  PRIVATE glfw glad OpenGL::GL
)

# ---------------------------------------------------------------------------
# Engine-wide helper (compile options, warnings, folder, etc.)
# ---------------------------------------------------------------------------

rs_third_party_setup(imgui_backend_glfw_opengl)

# ---------------------------------------------------------------------------
# IDE folder grouping
# ---------------------------------------------------------------------------

set_target_properties(imgui_backend_glfw_opengl PROPERTIES
  FOLDER "ThirdParty/ImGui/Backends"
)

# ---------------------------------------------------------------------------
# Outputs consumed by BackendRegistry
# ---------------------------------------------------------------------------

set(RS_IMGUI_BACKEND_TARGET         imgui_backend_glfw_opengl  CACHE INTERNAL "")
set(RS_IMGUI_BACKEND_EXTRA_INCLUDES "${IMGUI_BACKENDS_DIR}"    CACHE INTERNAL "")

message(STATUS "[rs] ImGui backend loaded: GLFW + OpenGL3")
