# ============================================================================
#  RAYSIM - IMGUI BACKEND: GLFW + OPENGL3
# ============================================================================
#  Description: Compiles imgui_impl_glfw and imgui_impl_opengl3 into a static
#               library. Exposes rs::imgui_backend as the canonical alias so
#               BackendRegistry can link it without knowing the concrete name.
#
#  Requires (set by add_imgui.cmake before this file is included):
#    IMGUI_DIR          - root of the ImGui source tree
#    IMGUI_BACKENDS_DIR - ImGui backends/ subdirectory
#
#  Requires (set by glfw_opengl.cmake before this file is included):
#    glfw               - CMake target
#    glad               - CMake target
#    OpenGL::GL         - CMake target
#
#  Copyright (c) 2026 Dayron Mustelier (@DMsuDev)
#  Licensed under the Apache License, Version 2.0.
# ============================================================================

include_guard()

# ----------------------------------------------------------------------------
# Precondition: add_imgui.cmake must have run first
# ----------------------------------------------------------------------------

foreach(_var IN ITEMS IMGUI_DIR IMGUI_BACKENDS_DIR)
  if(NOT DEFINED ${_var} OR "${${_var}}" STREQUAL "")
    message(FATAL_ERROR
      "[rs] imgui_glfw.cmake: '${_var}' is not set.\n"
      "     Include add_imgui.cmake (via ThirdParty.cmake) before loading any backend."
    )
  endif()
endforeach()

# ----------------------------------------------------------------------------
# Precondition: backend dependencies must already be targets
# ----------------------------------------------------------------------------

foreach(_target IN ITEMS glfw glad OpenGL::GL)
  if(NOT TARGET ${_target})
    message(FATAL_ERROR
      "[rs] imgui_glfw.cmake: target '${_target}' does not exist.\n"
      "     glfw_opengl.cmake must include() this file after its find_package/FetchContent calls."
    )
  endif()
endforeach()

# ----------------------------------------------------------------------------
# Static library: imgui_backend_glfw_opengl
# ----------------------------------------------------------------------------

add_library(imgui_backend_glfw_opengl STATIC
  "${IMGUI_BACKENDS_DIR}/imgui_impl_glfw.cpp"
  "${IMGUI_BACKENDS_DIR}/imgui_impl_opengl3.cpp"
)

add_library(rs::imgui_backend ALIAS imgui_backend_glfw_opengl)

target_include_directories(imgui_backend_glfw_opengl SYSTEM PUBLIC
  "${IMGUI_BACKENDS_DIR}"
  "${IMGUI_DIR}"
)

target_link_libraries(imgui_backend_glfw_opengl
  PUBLIC  imgui
  PRIVATE glfw glad OpenGL::GL
)

# ----------------------------------------------------------------------------
# Engine-wide third-party setup (silence warnings, etc.)
# ----------------------------------------------------------------------------

rs_third_party_setup(imgui_backend_glfw_opengl)

set_target_properties(imgui_backend_glfw_opengl PROPERTIES
  FOLDER "ThirdParty/ImGui/Backends"
)

message(STATUS "[rs] ImGui backend loaded: GLFW + OpenGL3")
