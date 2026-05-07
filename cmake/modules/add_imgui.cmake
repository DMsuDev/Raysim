# ===========================================================================
# Builds the backend-agnostic ImGui core library.
#
# The imgui backend glue (rlImGui, imgui_impl_glfw, ...) is NOT loaded here.
# Each cmake/Backends/<id>.cmake includes its own imgui backend after its
# own find_package() call, ensuring the windowing library already exists.
# ===========================================================================

include_guard()

# ===========================================================================
# Paths (used by imgui backend files included from Backends/*.cmake)
# ===========================================================================

set(IMGUI_DIR          ${CMAKE_SOURCE_DIR}/third_party/imgui)
set(IMGUI_BACKENDS_DIR ${IMGUI_DIR}/backends)

# ===========================================================================
# imgui core, backend-agnostic
# ===========================================================================

add_library(imgui STATIC
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
)

target_include_directories(imgui SYSTEM PUBLIC ${IMGUI_DIR})

rs_third_party_setup(imgui)
set_target_properties(imgui PROPERTIES FOLDER "ThirdParty/ImGui")
