# ===========================================================================
# ImGui backend: GLFW_OPENGL
# Uses imgui_impl_glfw + imgui_impl_opengl3.
#
# Required vcpkg packages : glfw3, OpenGL (system), glad (third_party)
# Sets:
#   RS_IMGUI_BACKEND_TARGET          - CMake target to link against
#   RS_IMGUI_BACKEND_EXTRA_INCLUDES  - additional private include dirs for Raysim
# ===========================================================================

include_guard()

add_library(imgui_backend_glfw_opengl STATIC
    ${IMGUI_BACKENDS_DIR}/imgui_impl_glfw.cpp
    ${IMGUI_BACKENDS_DIR}/imgui_impl_opengl3.cpp
)

target_include_directories(imgui_backend_glfw_opengl
    SYSTEM PUBLIC
    ${IMGUI_DIR}
    ${IMGUI_BACKENDS_DIR}
)

target_link_libraries(imgui_backend_glfw_opengl
    PUBLIC  imgui
    PRIVATE glfw glad OpenGL::GL
)

rs_third_party_setup(imgui_backend_glfw_opengl)

set(RS_IMGUI_BACKEND_TARGET         imgui_backend_glfw_opengl)
set(RS_IMGUI_BACKEND_EXTRA_INCLUDES ${IMGUI_BACKENDS_DIR})

# Grouping for IDEs
set_target_properties(imgui_backend_glfw_opengl PROPERTIES FOLDER "ThirdParty/ImGui/Backends")
