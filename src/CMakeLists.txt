# Create the library target
add_library(Raysim ${RS_LIB_TYPE})
add_library(Raysim::Raysim ALIAS Raysim)

# Group in IDEs
set_target_properties(Raysim PROPERTIES
    FOLDER "Raysim"
    OUTPUT_NAME "raysim"
)

# ============================================================
# SETTINGS
# ============================================================

rs_apply_build_defines(Raysim)

rs_enable_sanitizers(Raysim)
rs_enable_coverage(Raysim)
rs_enable_optimizations(Raysim)
rs_enable_lto(Raysim)
rs_enable_rdynamic(Raysim)

# Shared library symbol visibility
# RS_BUILD_SHARED: PUBLIC so consumers know to use dllimport
# RS_BUILD_DLL:    PRIVATE so only the library itself sets dllexport
target_compile_definitions(Raysim
    PUBLIC
        $<$<BOOL:${RS_BUILD_SHARED}>:RS_BUILD_SHARED>
    PRIVATE
        $<$<BOOL:${RS_BUILD_SHARED}>:RS_BUILD_DLL>
)

# Add precompiled headers for faster compilation
if(RS_ENABLE_PCH)
    target_precompile_headers(Raysim PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/rspch.hpp)
endif()

# ============================================================================
# RAYSIM CORE LIBRARY - SOURCE FILES
# ============================================================================

set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)

# Source files for the main library
set(RAYSIM_SOURCES
    # Graphics
    ${SRC_DIR}/Graphics/Shapes.cpp
    ${SRC_DIR}/Graphics/Texts.cpp
    ${SRC_DIR}/Renderer/RenderCommand.cpp

    ${SRC_DIR}/Debug/Instrumentor.cpp
    ${SRC_DIR}/Core/Log.cpp

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

    # Input abstraction layer
    ${SRC_DIR}/Input/EventDrivenInput.cpp

    # Backend: Raylib implementations
    ${SRC_DIR}/Backend/Raylib/RaylibInput.cpp
    ${SRC_DIR}/Backend/Raylib/RaylibRendererAPI.cpp
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
        $<BUILD_INTERFACE:${IMGUI_DIR}>
        $<INSTALL_INTERFACE:include>

    PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${IMGUI_DIR}
)

# ============================================================
# Link libraries
# ============================================================

target_link_libraries(Raysim
    PUBLIC
        spdlog # header-only INTERFACE
        # Static build: imgui + rs_backend propagate to the consumer EXE (it links the static libs).
        $<$<NOT:$<BOOL:${RS_BUILD_SHARED}>>:imgui>
        $<$<NOT:$<BOOL:${RS_BUILD_SHARED}>>:rs_backend>
    PRIVATE
        # Shared build: imgui + rs_backend are embedded in the DLL only.
        # MinGW exports all symbols by default, so consumer EXEs resolve ImGui::*
        # and raylib symbols from the DLL at runtime, no second copy of GImGui.
        $<$<BOOL:${RS_BUILD_SHARED}>:imgui>
        $<$<BOOL:${RS_BUILD_SHARED}>:rs_backend>
        rs_warnings
        stb
        $<$<PLATFORM_ID:Windows>:winmm>
)
