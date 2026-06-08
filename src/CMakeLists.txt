# ============================================================================
# Target declaration
#
# RS_LIB_TYPE is STATIC or SHARED, resolved by ProjectOptions.cmake based on
# the RS_BUILD_SHARED option. The alias Raysim::Raysim lets consumers use
# the namespaced form whether they find the library via add_subdirectory or
# find_package.
# ============================================================================

add_library(Raysim ${RS_LIB_TYPE})
add_library(Raysim::Raysim ALIAS Raysim)

set_target_properties(Raysim PROPERTIES
  FOLDER      "Raysim"
  OUTPUT_NAME "raysim" # produces raysim.dll / libraysim.so / raysim.lib
)

# ============================================================================
# Build settings
#
# These helpers are defined in cmake/ProjectOptions.cmake and apply compiler
# flags, sanitizer instrumentation, coverage, LTO, and -rdynamic (Unix only)
# based on the active CMake options.
# ============================================================================

rs_apply_build_defines(Raysim)

sanitizers_enable(Raysim)
rs_enable_coverage(Raysim)
rs_enable_optimizations(Raysim)
rs_enable_lto(Raysim)
rs_enable_rdynamic(Raysim)

# ============================================================================
# Symbol visibility macros
#
# RS_API (defined in include/Raysim/Core/API.hpp) controls
# dllexport / dllimport annotations on every public class and free function.
#
# API.hpp includes PlatformDetection.hpp transitively.
#
#   RS_BUILD_SHARED — PUBLIC: propagates to consumer targets so they know to
#                     use __declspec(dllimport) (Windows) or default visibility.
#   RS_BUILD_DLL    — PRIVATE: only the library itself gets dllexport; consumers
#                     must never see this definition or they would try to export.
# ============================================================================

target_compile_definitions(Raysim
  PUBLIC
    $<$<BOOL:${RS_BUILD_SHARED}>:RS_BUILD_SHARED>
    $<$<BOOL:${RS_BUILD_SHARED}>:IMGUI_BUILD_SHARED>
  PRIVATE
    $<$<BOOL:${RS_BUILD_SHARED}>:RS_BUILD_DLL>
    $<$<BOOL:${RS_BUILD_SHARED}>:IMGUI_BUILD_DLL>
)

# ============================================================================
# Precompiled header
#
# rspch.hpp pulls in the most-used standard and third-party headers, reducing
# incremental build times significantly on large rebuilds.
# Disabled when RS_ENABLE_PCH=OFF (e.g. for CI cache-miss analysis).
# ============================================================================

if(RS_ENABLE_PCH)
  target_precompile_headers(Raysim PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/rspch.hpp)
endif()

# ============================================================================
# Source files
# ============================================================================

set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)

set(RAYSIM_SOURCES
  # --- Graphics & Renderer -------------------------------------------------
  ${SRC_DIR}/Graphics/Shapes.cpp # 2D shape drawing helpers
  ${SRC_DIR}/Graphics/Texts.cpp # text rendering helpers
  ${SRC_DIR}/Renderer/RenderCommand.cpp # backend-agnostic render API

  # --- Debug ---------------------------------------------------------------
  ${SRC_DIR}/Debug/Instrumentor.cpp # Chrome-trace profiler

  # --- Core ----------------------------------------------------------------
  ${SRC_DIR}/Core/Log.cpp # spdlog wrapper (RS::Log)
  ${SRC_DIR}/Core/BackendFactory.cpp # creates the active IBackend
  ${SRC_DIR}/Core/Time.cpp # frame-time / delta-time
  ${SRC_DIR}/Core/Application.cpp # main application loop
  ${SRC_DIR}/Core/LayerStack.cpp # ordered layer container

  # --- Math ----------------------------------------------------------------
  ${SRC_DIR}/Math/Random.cpp # PRNG + noise functions

  # --- Scene ---------------------------------------------------------------
  ${SRC_DIR}/Scene/SceneManager.cpp # scene lifecycle management
  ${SRC_DIR}/Scene/Scene.cpp # base Scene class

  # --- Input ---------------------------------------------------------------
  ${SRC_DIR}/Input/EventDrivenInput.cpp # event-queue input adapter

  # --- Backend: Raylib -----------------------------------------------------
  ${SRC_DIR}/Backend/Raylib/RaylibInput.cpp
  ${SRC_DIR}/Backend/Raylib/RaylibRendererAPI.cpp
  ${SRC_DIR}/Backend/Raylib/RaylibWindow.cpp
  ${SRC_DIR}/Backend/Raylib/RaylibFontRenderer.cpp
  ${SRC_DIR}/Backend/Raylib/RaylibImGuiBackend.cpp

  # --- ImGui layer ---------------------------------------------------------
  ${SRC_DIR}/ImGui/ImGuiLayer.cpp # wraps Dear ImGui begin/end frame

  # --- Font system ---------------------------------------------------------
  ${SRC_DIR}/Fonts/Providers/STBTrueTypeProvider.cpp
  ${SRC_DIR}/Fonts/FontManager.cpp

  # --- Utilities -----------------------------------------------------------
  ${SRC_DIR}/Utils/PathUtils.cpp
)

target_sources(Raysim PRIVATE ${RAYSIM_SOURCES})

# ============================================================================
# Include directories
#
#   PUBLIC  (BUILD_INTERFACE)  — needed by Raysim itself and all consumers
#     include/   : public headers (RS::Application, RS::Scene, …)
#     IMGUI_DIR  : imgui.h, imgui_internal.h  (consumers call ImGui:: directly)
#
#   PUBLIC  (INSTALL_INTERFACE) — path exposed when Raysim is installed via
#                                  cmake --install; relative to install prefix.
#
#   PRIVATE — internal implementation details, not exposed to consumers
#     src/       : internal headers (not part of the public API)
#     IMGUI_DIR  : redundant for consumers but required for PRIVATE .cpp files
#                  when imgui is PRIVATE-linked (shared build).
# ============================================================================

target_include_directories(Raysim
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${IMGUI_DIR}>
    $<INSTALL_INTERFACE:include>
  PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${IMGUI_DIR}
)

# ============================================================================
# Link libraries
#
# Static build (RS_BUILD_SHARED=OFF)
#   imgui and rs_backend are PUBLIC so the consumer EXE also links them
#   (static libs must be linked by the final binary that uses them).
#
# Shared build (RS_BUILD_SHARED=ON)
#   imgui and rs_backend are PRIVATE — their object code is baked into
#   raysim.dll.  Consumer EXEs do NOT re-link them; they resolve all symbols
#   through the import library (raysim.lib) at link time and raysim.dll at
#   runtime.  MinGW/GNU ld exports all DLL symbols by default; lld-link
#   requires explicit __declspec(dllexport) — handled via imgui_config.hpp
#   (see add_imgui.cmake and the IMGUI_USER_CONFIG section below).
#
# Always PRIVATE
#   rs_warnings  : compiler warning flags, never leaked to consumers.
#   stb          : header-only implementation TUs compiled once here.
#   winmm        : Windows multimedia timer (timeBeginPeriod); not needed
#                  on other platforms.
# ============================================================================

target_link_libraries(Raysim
  PUBLIC
    spdlog # header-only INTERFACE target
    $<$<NOT:$<BOOL:${RS_BUILD_SHARED}>>:imgui> # static build only
    $<$<NOT:$<BOOL:${RS_BUILD_SHARED}>>:rs::backend> # static build only
  PRIVATE
    $<$<BOOL:${RS_BUILD_SHARED}>:imgui> # shared build: embedded in DLL
    $<$<BOOL:${RS_BUILD_SHARED}>:rs::backend> # shared build: embedded in DLL
    rs_warnings
    stb
    $<$<PLATFORM_ID:Windows>:winmm>
)

# ============================================================================
# IMGUI_USER_CONFIG — propagate to consumers
#
# add_imgui.cmake already sets IMGUI_USER_CONFIG on the 'imgui' target (PUBLIC)
# so the Raysim library itself sees it.  However, in a shared build 'imgui' is
# PRIVATE-linked, meaning its PUBLIC compile definitions do NOT propagate to
# consumer EXEs.  We therefore re-apply IMGUI_USER_CONFIG on Raysim PUBLIC so
# that every consumer TU that includes imgui.h uses the same imconfig and gets
# the matching __declspec(dllimport) annotation for IMGUI_API.
#
# RS_BUILD_SHARED and IMGUI_BUILD_SHARED are already PUBLIC on Raysim (see Symbol visibility section),
# so consumers automatically pick up the dllimport branch in imgui_config.hpp.
# ============================================================================

target_compile_definitions(Raysim PUBLIC
  IMGUI_USER_CONFIG="Raysim/imgui_config.hpp"
)
