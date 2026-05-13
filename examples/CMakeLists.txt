# === Raysim Examples CMakeLists.txt ===
# This CMakeLists.txt is responsible for building the examples.

include_guard()

# =======================================================
# Base Path.
# =======================================================

set(EXAMPLES_ASSETS_SOURCE ${CMAKE_SOURCE_DIR}/examples/shared/assets)

# =======================================================
# Destination for shared assets.
# =======================================================

set(ASSETS_DEST "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets")

# =======================================================
# Asset copying
# =======================================================

add_custom_target(copy_shared_assets ALL
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${EXAMPLES_ASSETS_SOURCE} ${ASSETS_DEST}
  COMMENT "Copying shared assets to output directory"
)

# =======================================================
# List of example subdirectories.
# =======================================================

set(RS_EXAMPLES
    LissajousCurves
    Mouse2D
    NoiseLandscape
    BouncingBalls
    SceneShowcase
)

foreach(example IN LISTS RS_EXAMPLES)
  add_subdirectory(${example})
endforeach()

# =======================================================
# Make all example targets depend on the asset copying target.
# This ensures assets are copied before any example runs.
# =======================================================

foreach(example IN LISTS RS_EXAMPLES)
  add_dependencies(${example} copy_shared_assets)
endforeach()
