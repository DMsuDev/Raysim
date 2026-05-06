include_guard()

# Destination for shared assets. If the generator is multi-config, place
# assets under a config subfolder (e.g. Debug/Release) using $<CONFIG>.
if(CMAKE_CONFIGURATION_TYPES)
    set(ASSETS_DEST "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<CONFIG>/assets")
else()
    set(ASSETS_DEST "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets")
endif()

add_custom_target(SharedAssets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/examples/shared/assets
        ${ASSETS_DEST}
    COMMENT "Copying shared assets for examples"
)

message(STATUS "Building Raysim examples...")

# Other way to add examples:
# - Automatically find all subdirectories in the examples folder
# - Each subdirectory is expected to have its own CMakeLists.txt that defines the example

# file(GLOB EXAMPLE_DIRS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *)
#
# foreach(dir ${EXAMPLE_DIRS})
#     if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${dir})
#         add_subdirectory(${dir})
#     endif()
# endforeach()

add_subdirectory(LissajousCurves)
add_subdirectory(Mouse2D)
add_subdirectory(NoiseLandscape)
add_subdirectory(BouncingBalls)
add_subdirectory(SceneShowcase)
