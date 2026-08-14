# ==============================================================================
#  RAYSIM - EXAMPLES BUILDER
# ==============================================================================
#  Description: Builds all Raysim examples and manages shared assets.
#
#  Copyright (c) 2026 Dayron Mustelier (@DMsuDev)
#  Licensed under the Apache License, Version 2.0.
# ==============================================================================

include_guard()

# ------------------------------------------------------------------------------
#  Shared Assets Directories
# ------------------------------------------------------------------------------
set(EXAMPLES_ASSETS_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/shared/assets")
set(EXAMPLES_ASSETS_DEST   "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets")

# ------------------------------------------------------------------------------
#  Assets Copy Target
# ------------------------------------------------------------------------------
add_custom_target(copy_shared_assets ALL
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${EXAMPLES_ASSETS_SOURCE}"
    "${EXAMPLES_ASSETS_DEST}"
  COMMENT "Copying shared assets to output directory"
)

# ------------------------------------------------------------------------------
#  Example Targets
# ------------------------------------------------------------------------------
set(RS_EXAMPLES
  LissajousCurves
  Mouse2D
  NoiseLandscape
  BouncingBalls
  SceneShowcase
)

foreach(example IN LISTS RS_EXAMPLES)
  add_subdirectory(${example})
  # Add dependency to ensure shared assets are copied before building the example
  add_dependencies(${example} copy_shared_assets)
endforeach()
