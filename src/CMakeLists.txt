# Create library target
add_library(${PROJECT_NAME} STATIC)

# Add precompiled headers for faster compilation
target_precompile_headers(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/pch.hpp)

# ============================================================================
# RAYSIM CORE LIBRARY - SOURCE FILES
# ============================================================================

# Source files for the main library
target_sources(${PROJECT_NAME} PRIVATE
    # Graphics
    Graphics/Color.cpp
    Graphics/Shapes.cpp
    Graphics/Texts.cpp

    # Core managers
    Core/FontManager.cpp
    Core/BackendFactory.cpp
    Core/Time.cpp
    Core/Logger.cpp
    Core/Application.cpp

    # Math
    Math/Random.cpp

    # Backend: Raylib implementations
    Backend/Raylib/RaylibInput.cpp
    Backend/Raylib/RaylibRenderer.cpp
    Backend/Raylib/RaylibWindow.cpp
)

# ============================================================================
# INCLUDE DIRECTORIES
# ============================================================================

# Public include directory for headers
target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

# ============================================================================
# LINK LIBRARIES
# ============================================================================

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        raylib
        spdlog::spdlog_header_only
)

target_compile_definitions(${PROJECT_NAME}
    PRIVATE
        $<$<CONFIG:Debug>:RS_DEBUG>
)

# MSVC requires /Zc:preprocessor for __VA_OPT__ support (used in Assert.hpp)
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /Zc:preprocessor)
endif()
