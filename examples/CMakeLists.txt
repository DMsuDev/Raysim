# ==============================================================================
#  RAYSIM - EXAMPLES BUILDER
# ==============================================================================
#  Description: Builds all Raysim examples and manages shared assets.
#
#  Copyright (c) 2026 Dayron Mustelier (@DMsuDev)
#  Licensed under the Apache License, Version 2.0.
# ==============================================================================

# ------------------------------------------------------------------------------
#  Shared Assets Directories
# ------------------------------------------------------------------------------
set(EXAMPLES_ASSETS_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/resources/assets")
set(EXAMPLES_ASSETS_DEST   "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets")

# ------------------------------------------------------------------------------
#  Assets Copy Target
# ------------------------------------------------------------------------------
add_custom_target(copy_shared_assets ALL
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${EXAMPLES_ASSETS_SOURCE}"
    "${EXAMPLES_ASSETS_DEST}"
  COMMENT "[rs] Copying shared assets to output directory"
)

# ------------------------------------------------------------------------------
#  Example Targets
# ------------------------------------------------------------------------------

# Include helper functions for building examples
include("${CMAKE_CURRENT_SOURCE_DIR}/resources/cmake/ExampleHelpers.cmake")

set(RS_EXAMPLES
  demos/LissajousCurves
  demos/Mouse2D
  demos/NoiseLandscape
  demos/BouncingBalls
  demos/SceneShowcase
)

foreach(example IN LISTS RS_EXAMPLES)
  # Extract just the target name (last path component)
  cmake_path(GET example FILENAME _example_name)

  add_subdirectory(${example})

  # Ensure shared assets are copied before building the example
  add_dependencies(${_example_name} copy_shared_assets)
endforeach()
