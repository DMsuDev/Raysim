include_guard()

# ============================================================
# Paths
# ============================================================
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/third_party/imgui)
set(IMGUI_BACKENDS_DIR ${IMGUI_DIR}/backends)

# Core sources
set(IMGUI_CORE_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
)

# ============================================================
# ImGui Core
# ============================================================
add_library(imgui STATIC ${IMGUI_CORE_SOURCES})

target_include_directories(imgui
    SYSTEM PUBLIC
    ${IMGUI_DIR}
)

rs_third_party_setup(imgui)

# ============================================================
# ImGui Backends
# ============================================================
add_library(imgui_glfw_opengl STATIC
    ${IMGUI_BACKENDS_DIR}/imgui_impl_glfw.cpp
    ${IMGUI_BACKENDS_DIR}/imgui_impl_opengl3.cpp
)

target_include_directories(imgui_glfw_opengl
    SYSTEM PUBLIC
    ${IMGUI_DIR}
    ${IMGUI_BACKENDS_DIR}
)

target_link_libraries(imgui_glfw_opengl
    PUBLIC imgui
    PRIVATE glfw glad OpenGL::GL
)

rs_third_party_setup(imgui_glfw_opengl)

# ============================================================
# rlImGui (Raylib ImGui integration)
# ============================================================
set(RLIMGUI_DIR ${CMAKE_SOURCE_DIR}/third_party/rlImGui)

add_library(rlimgui STATIC
    ${RLIMGUI_DIR}/rlImGui.cpp
)

target_include_directories(rlimgui
    SYSTEM PUBLIC
    ${RLIMGUI_DIR}
    ${IMGUI_DIR}
)

target_link_libraries(rlimgui
    PUBLIC imgui raylib
)

rs_third_party_setup(rlimgui)
