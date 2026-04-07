include_guard()

set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/third_party/imgui)

# ============================================================
# CORE
# ============================================================

add_library(imgui STATIC
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
)

# System include to avoid warnings from ImGui's code when compiling with -Wall -Wextra
target_include_directories(imgui SYSTEM PUBLIC ${IMGUI_DIR})

disable_warnings(imgui)

# ============================================================
# BACKENDS
# ============================================================

add_library(imgui_backends STATIC
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui_backends
    SYSTEM PUBLIC
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

target_link_libraries(imgui_backends
    PRIVATE
    imgui
    glfw
    glad
    OpenGL::GL
)

disable_warnings(imgui_backends)
