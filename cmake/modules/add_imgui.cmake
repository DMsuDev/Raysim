include_guard()

# ============================================================
# Paths
# ============================================================
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/third_party/imgui)

# Core sources
set(IMGUI_CORE_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
)

# Backend sources
set(IMGUI_BACKEND_SOURCES
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

# ============================================================
# ImGui Core
# ============================================================
add_library(imgui STATIC ${IMGUI_CORE_SOURCES})

target_include_directories(imgui
    SYSTEM PUBLIC
    ${IMGUI_DIR}
)

disable_warnings(imgui)

# ============================================================
# ImGui Backends
# ============================================================
add_library(imgui_backends STATIC ${IMGUI_BACKEND_SOURCES})

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

# Grouping in IDEs
set_target_properties(imgui PROPERTIES FOLDER "third_party")
set_target_properties(imgui_backends PROPERTIES FOLDER "third_party")
