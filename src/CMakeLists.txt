# Create the library target
add_library(Raysim ${RS_LIB_TYPE})
add_library(Raysim::Raysim ALIAS Raysim)

# Group in IDEs
set_target_properties(Raysim PROPERTIES FOLDER "Raysim")

# ============================================================
# SETTINGS
# ============================================================

rs_apply_build_defines(Raysim)

rs_enable_sanitizers(Raysim)
rs_enable_coverage(Raysim)
rs_enable_optimizations(Raysim)
rs_enable_lto(Raysim)

# Add precompiled headers for faster compilation
if(RS_ENABLE_PCH)
    target_precompile_headers(Raysim PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/pch.hpp)
endif()

# ============================================================================
# RAYSIM CORE LIBRARY - SOURCE FILES
# ============================================================================

set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)

# Source files for the main library
set(RAYSIM_SOURCES
    # Graphics
    ${SRC_DIR}/Graphics/Color.cpp
    ${SRC_DIR}/Graphics/Shapes.cpp
    ${SRC_DIR}/Graphics/Texts.cpp

    ${SRC_DIR}/Debug/Instrumentor.cpp
    ${SRC_DIR}/Core/Log.cpp

    ${SRC_DIR}/Renderer/RenderCommand.cpp

    # Core managers
    ${SRC_DIR}/Core/BackendFactory.cpp
    ${SRC_DIR}/Core/Time.cpp
    ${SRC_DIR}/Core/Application.cpp
    ${SRC_DIR}/Core/LayerStack.cpp

    # Math
    ${SRC_DIR}/Math/Random.cpp

    # Scenes
    ${SRC_DIR}/Scene/SceneManager.cpp
    ${SRC_DIR}/Scene/Scene.cpp

    # Backend: Raylib implementations
    ${SRC_DIR}/Backend/Raylib/RaylibInput.cpp
    ${SRC_DIR}/Backend/Raylib/RaylibRendererAPI.cpp
    ${SRC_DIR}/Backend/Raylib/RaylibTexture.cpp
    ${SRC_DIR}/Backend/Raylib/RaylibWindow.cpp
    ${SRC_DIR}/Backend/Raylib/RaylibFontRenderer.cpp
    ${SRC_DIR}/Backend/Raylib/RaylibImGuiBackend.cpp

    # ImGui layer
    ${SRC_DIR}/ImGui/ImGuiLayer.cpp

    # Font system
    ${SRC_DIR}/Fonts/Providers/STBTrueTypeProvider.cpp
    ${SRC_DIR}/Fonts/FontManager.cpp

    ${SRC_DIR}/Utils/PathUtils.cpp
)

# Add the sources to the library target
target_sources(Raysim PRIVATE ${RAYSIM_SOURCES})

# ============================================================================
# INCLUDE DIRECTORIES
# ============================================================================

# Set the target include directories
target_include_directories(Raysim
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>

    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${IMGUI_DIR}
        ${IMGUI_DIR}/backends
        ${RLIMGUI_DIR}
)

# ============================================================
# Link libraries
# ============================================================

target_link_libraries(Raysim
    PUBLIC
        imgui
        spdlog
        $<$<BOOL:${RS_BUILD_SHARED}>:raylib>
        $<$<BOOL:${RS_BUILD_SHARED}>:glfw>
        $<$<BOOL:${RS_BUILD_SHARED}>:glad>
        $<$<BOOL:${RS_BUILD_SHARED}>:imgui_glfw_opengl>
        $<$<BOOL:${RS_BUILD_SHARED}>:rlimgui>
        $<$<BOOL:${RS_BUILD_SHARED}>:stb>
    PRIVATE
        rs_warnings
        $<$<NOT:$<BOOL:${RS_BUILD_SHARED}>>:raylib>
        $<$<NOT:$<BOOL:${RS_BUILD_SHARED}>>:glfw>
        $<$<NOT:$<BOOL:${RS_BUILD_SHARED}>>:glad>
        $<$<NOT:$<BOOL:${RS_BUILD_SHARED}>>:imgui_glfw_opengl>
        $<$<NOT:$<BOOL:${RS_BUILD_SHARED}>>:rlimgui>
        $<$<NOT:$<BOOL:${RS_BUILD_SHARED}>>:stb>
        $<$<PLATFORM_ID:Windows>:winmm>
)
