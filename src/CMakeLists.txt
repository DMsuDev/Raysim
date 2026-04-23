# Create the library target
add_library(Raysim)

# Create an alias for the library target to allow for easier linking
add_library(Raysim::Raysim ALIAS Raysim)

# ============================================================
# SETTINGS
# ============================================================

set_project_warnings(Raysim)
enable_coverage(Raysim)
enable_sanitizers(Raysim)

# Add precompiled headers for faster compilation
target_precompile_headers(Raysim PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/pch.hpp)

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

    # Core managers
    ${SRC_DIR}/Core/FontManager.cpp
    ${SRC_DIR}/Core/BackendFactory.cpp
    ${SRC_DIR}/Core/Time.cpp
    ${SRC_DIR}/Core/Log.cpp
    ${SRC_DIR}/Core/Application.cpp

    # Math
    ${SRC_DIR}/Math/Random.cpp

    # Scenes
    ${SRC_DIR}/Scene/SceneManager.cpp

    # Backend: Raylib implementations
    ${SRC_DIR}/Backend/Raylib/RaylibInput.cpp
    ${SRC_DIR}/Backend/Raylib/RaylibRendererAPI.cpp
    ${SRC_DIR}/Backend/Raylib/RaylibWindow.cpp
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
        ${IMGUI_DIR}
        ${IMGUI_DIR}/backends
)

# ============================================================
# Link libraries
# ============================================================

target_link_libraries(Raysim
    PUBLIC
        imgui
        glfw
        raylib
        spdlog::spdlog
    PRIVATE
        glad
        imgui_backends
        $<$<PLATFORM_ID:Windows>:winmm>
)
