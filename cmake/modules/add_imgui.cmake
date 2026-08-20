# ============================================================================
#  RAYSIM - ADD IMGUI
# ============================================================================
#  Description: Builds the backend-agnostic ImGui core as a STATIC target.
#
#  Sets (for use by imgui backend files):
#    IMGUI_DIR          - root of the ImGui source tree
#    IMGUI_BACKENDS_DIR - backends/ subdirectory (impl_glfw, impl_opengl3, …)
#
#  Copyright (c) 2026 Dayron Mustelier (@DMsuDev)
#  Licensed under the Apache License, Version 2.0.
# ============================================================================

include_guard()

# ----------------------------------------------------------------------------
# Paths
# ----------------------------------------------------------------------------

set(IMGUI_DIR          "${PROJECT_SOURCE_DIR}/third_party/imgui")
set(IMGUI_BACKENDS_DIR "${IMGUI_DIR}/backends")

# ----------------------------------------------------------------------------
# Target: imgui (STATIC)
# ----------------------------------------------------------------------------

add_library(imgui STATIC
  "${IMGUI_DIR}/imgui.cpp"
  "${IMGUI_DIR}/imgui_draw.cpp"
  "${IMGUI_DIR}/imgui_demo.cpp"
  "${IMGUI_DIR}/imgui_tables.cpp"
  "${IMGUI_DIR}/imgui_widgets.cpp"
)

target_include_directories(imgui SYSTEM PUBLIC
  "${IMGUI_DIR}"
  "${PROJECT_SOURCE_DIR}/include"   # resolves "Raysim/imgui_config.hpp"
)

# IMGUI_USER_CONFIG hooks into imgui.h to customise IMGUI_API for DLL builds.
target_compile_definitions(imgui PUBLIC
  IMGUI_USER_CONFIG="Raysim/imgui_config.hpp"
)

if(RS_BUILD_SHARED)
  target_compile_definitions(imgui PRIVATE
    IMGUI_BUILD_SHARED  # triggers shared-build branch in imgui_config.hpp
    IMGUI_BUILD_DLL     # emits __declspec(dllexport) for ImGui symbols
  )
endif()

# ----------------------------------------------------------------------------
# Engine-wide third-party setup (silence warnings, etc.)
# ----------------------------------------------------------------------------

rs_third_party_setup(imgui)

set_target_properties(imgui PROPERTIES
  FOLDER "ThirdParty/ImGui"
)
