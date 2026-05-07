# ===========================================================================
# ImGui backend: RAYLIB
# Uses rlImGui as the Raylib+ImGui glue layer.
#
# Required vcpkg packages : raylib (already resolved in CMakeLists.txt)
# Sets:
#   RS_IMGUI_BACKEND_TARGET          - CMake target to link against
#   RS_IMGUI_BACKEND_EXTRA_INCLUDES  - additional private include dirs for Raysim
# ===========================================================================

include_guard()

set(RLIMGUI_DIR ${CMAKE_SOURCE_DIR}/third_party/rlImGui)

add_library(imgui_backend_raylib STATIC
    ${RLIMGUI_DIR}/rlImGui.cpp
)

target_include_directories(imgui_backend_raylib
    SYSTEM PUBLIC
    ${RLIMGUI_DIR}
    ${IMGUI_DIR}
)

target_link_libraries(imgui_backend_raylib
    PUBLIC imgui raylib
)

rs_third_party_setup(imgui_backend_raylib)

set(RS_IMGUI_BACKEND_TARGET         imgui_backend_raylib)
set(RS_IMGUI_BACKEND_EXTRA_INCLUDES ${RLIMGUI_DIR})

# Grouping for IDEs
set_target_properties(imgui_backend_raylib PROPERTIES FOLDER "ThirdParty/ImGui/Backends")
