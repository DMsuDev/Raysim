# ===========================================================================
# Builds the backend-agnostic ImGui core library as a STATIC target named
# 'imgui'.  This module is included exactly once (include_guard) and is
# responsible only for the core translation units:
#
#   imgui.cpp  imgui_draw.cpp  imgui_demo.cpp
#   imgui_tables.cpp  imgui_widgets.cpp
#
# Backend-specific glue (rlImGui, imgui_impl_glfw, imgui_impl_opengl3, …)
# is NOT added here.  Each cmake/Backends/<id>.cmake file appends its own
# backend sources and links after its find_package() calls, so that the
# windowing library is already available when the backend glue is compiled.
# ===========================================================================

include_guard()

# ===========================================================================
# Paths
#
# IMGUI_DIR          -> root of the vendored ImGui source tree.
# IMGUI_BACKENDS_DIR -> backends/ sub-directory; referenced by individual
#                       backend cmake files (Backends/*.cmake) to locate
#                       imgui_impl_*.cpp / *.h files.
# ===========================================================================

set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/third_party/imgui)
set(IMGUI_BACKENDS_DIR ${IMGUI_DIR}/backends)

# ===========================================================================
# Target: imgui (STATIC)
#
# Only the five backend-agnostic translation units are compiled here.
# ===========================================================================

add_library(imgui STATIC
  ${IMGUI_DIR}/imgui.cpp
  ${IMGUI_DIR}/imgui_draw.cpp
  ${IMGUI_DIR}/imgui_demo.cpp
  ${IMGUI_DIR}/imgui_tables.cpp
  ${IMGUI_DIR}/imgui_widgets.cpp
)

# ===========================================================================
# Include directories
#
# IMGUI_DIR is PUBLIC so that backend cmake files and any target that links
# imgui can #include <imgui.h> without repeating the path.
#
# CMAKE_SOURCE_DIR/include is added so imgui can resolve the user-config
# header via the short path "Raysim/imgui_config.hpp" (see below).
# ===========================================================================

target_include_directories(imgui SYSTEM PUBLIC
  ${IMGUI_DIR}
  ${CMAKE_SOURCE_DIR}/include
)

# ===========================================================================
# IMGUI_USER_CONFIG - custom imconfig hook
#
# ImGui includes IMGUI_USER_CONFIG before anything else (imgui.h line 66),
# letting us override IMGUI_API without touching third-party source files.
#
# include/Raysim/imgui_config.hpp defines IMGUI_API as:
#   • __declspec(dllexport)  when RS_BUILD_DLL is defined  (DLL build)
#   • __declspec(dllimport)  when RS_BUILD_SHARED is defined (consumer)
#   • __attribute__((visibility("default")))  on non-Windows shared builds
#   • (empty)                for static builds
#
# The definition is PUBLIC so that any target linking imgui automatically
# uses the same imconfig (consistent IMGUI_API across all TUs).
# ===========================================================================

target_compile_definitions(imgui PUBLIC
  IMGUI_USER_CONFIG="Raysim/imgui_config.hpp"
)

# ===========================================================================
# Shared-library symbol visibility
#
# When Raysim is built as a shared library (RS_BUILD_SHARED=ON), imgui's
# object code is embedded directly inside raysim.dll (PRIVATE link).
# We must compile imgui with IMGUI_BUILD_DLL so imgui_config.hpp emits
# __declspec(dllexport) and lld-link places ImGui symbols in the DLL's
# export table.  Without this, consumer EXEs cannot resolve ImGui::* calls.
#
# IMGUI_BUILD_SHARED is also set PRIVATE here because imgui is an internal
# detail of the DLL; the Raysim target already propagates IMGUI_BUILD_SHARED
# PUBLIC to its own consumers.
# ===========================================================================

if(RS_BUILD_SHARED)
  target_compile_definitions(imgui
    PRIVATE
      IMGUI_BUILD_SHARED # triggers shared-build branch in imconfig
      IMGUI_BUILD_DLL # triggers dllexport in imconfig
  )
endif()

# ---------------------------------------------------------------------------
# Engine-wide third-party setup (warnings, compile options, etc.)
# ---------------------------------------------------------------------------

rs_third_party_setup(imgui)

# ---------------------------------------------------------------------------
# IDE folder grouping
# ---------------------------------------------------------------------------

set_target_properties(imgui PROPERTIES
  FOLDER "ThirdParty/ImGui"
)

message(STATUS "[rs] ImGui core library loaded: (${IMGUI_DIR})")
