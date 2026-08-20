# ============================================================================
#  RAYSIM - BUILD CONFIGURATION
# ============================================================================
#  Description: CMake build configuration for the Raysim library.
#
#  Copyright (c) 2026 Dayron Mustelier (@DMsuDev)
#  Licensed under the Apache License, Version 2.0.
# ============================================================================

# ------------------------------------------------------------------------------
#  Source and Include Directories
# ------------------------------------------------------------------------------
set(RAYSIM_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
set(RAYSIM_GENERATED_INCLUDE_DIR "${PROJECT_BINARY_DIR}/include")

set(RAYSIM_SOURCES
  # --- Graphics & Renderer -------------------------------------------------
  Graphics/Shapes.cpp # 2D shape drawing helpers
  Graphics/Texts.cpp # text rendering helpers
  Renderer/RenderCommand.cpp # backend-agnostic render API

  # --- Debug ---------------------------------------------------------------
  Debug/Instrumentor.cpp # Chrome-trace profiler

  # --- Core ----------------------------------------------------------------
  Core/Log.cpp # spdlog wrapper (RS::Log)
  Core/BackendFactory.cpp # creates the active IBackend
  Core/Time.cpp # frame-time / delta-time
  Core/Application.cpp # main application loop
  Core/LayerStack.cpp # ordered layer container

  # --- Math ----------------------------------------------------------------
  Math/Random.cpp # PRNG + noise functions

  # --- Scene ---------------------------------------------------------------
  Scene/SceneManager.cpp # scene lifecycle management
  Scene/Scene.cpp # base Scene class

  # --- Input ---------------------------------------------------------------
  Input/EventDrivenInput.cpp # event-queue input adapter

  # --- Backend: Raylib -----------------------------------------------------
  Backend/Raylib/RaylibInput.cpp
  Backend/Raylib/RaylibRendererAPI.cpp
  Backend/Raylib/RaylibWindow.cpp
  Backend/Raylib/RaylibFontRenderer.cpp
  Backend/Raylib/RaylibImGuiBackend.cpp

  # --- ImGui layer ---------------------------------------------------------
  ImGui/ImGuiLayer.cpp # wraps Dear ImGui begin/end frame

  # --- Font system ---------------------------------------------------------
  Fonts/Providers/STBTrueTypeProvider.cpp
  Fonts/FontManager.cpp

  # --- Utilities -----------------------------------------------------------
  Utils/PathUtils.cpp
)

# ------------------------------------------------------------------------------
# Target
# ------------------------------------------------------------------------------
if(RS_BUILD_SHARED)
  message(STATUS "[rs] Building shared library")
  add_library(Raysim SHARED ${RAYSIM_SOURCES})
else()
  message(STATUS "[rs] Building static library")
  add_library(Raysim STATIC ${RAYSIM_SOURCES})
endif()

add_library(Raysim::Raysim ALIAS Raysim)

# ------------------------------------------------------------------------------
#  Header Include Directories Resolution
# ------------------------------------------------------------------------------
target_include_directories(Raysim
  PUBLIC
    $<BUILD_INTERFACE:${RAYSIM_INCLUDE_DIR}>
    $<BUILD_INTERFACE:${RAYSIM_GENERATED_INCLUDE_DIR}>
    $<BUILD_INTERFACE:${IMGUI_DIR}>
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${IMGUI_DIR}
)

# ------------------------------------------------------------------------------
#  Target Language Capabilities & Properties
# ------------------------------------------------------------------------------
target_compile_features(Raysim PUBLIC cxx_std_20)

set_target_properties(Raysim PROPERTIES
  CXX_EXTENSIONS OFF
  FOLDER         "Raysim"
  OUTPUT_NAME    "raysim" # produces raysim.dll / libraysim.so / raysim.lib
)

# ------------------------------------------------------------------------------
#  Precompiled header
# ------------------------------------------------------------------------------

target_precompile_headers(Raysim
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/rspch.hpp
)

# ------------------------------------------------------------------------------
#  Preprocessor Compile Definitions
# ------------------------------------------------------------------------------
target_compile_definitions(Raysim
  PUBLIC
    $<$<BOOL:${RS_BUILD_SHARED}>:RS_BUILD_SHARED>
    $<$<BOOL:${RS_BUILD_SHARED}>:IMGUI_BUILD_SHARED>

    $<$<CONFIG:Debug,RelWithDebInfo>:RS_BUILD_DEBUG>
    $<$<AND:$<BOOL:${RS_ENABLE_PROFILE}>,$<CONFIG:Debug,RelWithDebInfo>>:RS_ENABLE_PROFILE>
  PRIVATE
    $<$<BOOL:${RS_BUILD_SHARED}>:RS_BUILD_DLL>
    $<$<BOOL:${RS_BUILD_SHARED}>:IMGUI_BUILD_DLL>
)

# ------------------------------------------------------------------------------
#  Dependencies and Linkage
# ------------------------------------------------------------------------------
target_link_libraries(Raysim
  PUBLIC
    spdlog
    # Static builds expose transitive dependencies to consumers.
    $<$<NOT:$<BOOL:${RS_BUILD_SHARED}>>:imgui>
    $<$<NOT:$<BOOL:${RS_BUILD_SHARED}>>:rs::backend>

  PRIVATE
    $<$<BOOL:${RS_BUILD_SHARED}>:imgui>
    $<$<BOOL:${RS_BUILD_SHARED}>:rs::backend>
    rs_warnings
    stb
    $<$<PLATFORM_ID:Windows>:winmm>
)

# ------------------------------------------------------------------------------
# ImGui User Configuration
# ------------------------------------------------------------------------------
#  Provides Raysim-specific configuration macros for Dear ImGui.
#  Avoid modifying the imgui.h file directly, as it is part of the Dear ImGui library.
# ------------------------------------------------------------------------------

target_compile_definitions(Raysim PUBLIC
  IMGUI_USER_CONFIG="Raysim/imgui_config.hpp"
)

# ------------------------------------------------------------------------------
#  Compiler flags, sanitizers, coverage, optimizations, LTO, PCH, etc
# ------------------------------------------------------------------------------

rs_set_compiler_options(Raysim)
sanitizers_enable(Raysim)
rs_enable_coverage(Raysim)
rs_enable_optimizations(Raysim)
rs_enable_lto(Raysim)
