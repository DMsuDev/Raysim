# ==============================================================================
#  RAYSIM EXAMPLES - Noise Landscape
# ============================================================================
#  Copyright (c) 2026 @DMsuDev. Licensed under the Apache License, Version 2.0.
#  See LICENSE file in the project root for full license text.
# ==============================================================================

set(PROJECT_NAME "NoiseLandscape")

add_executable(${PROJECT_NAME} main.cpp SkyLayer.cpp TerrainLayer.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE Raysim::Raysim)

rs_example_setup(NoiseLandscape
  NAME        "Noise Landscape"
  DESCRIPTION "Procedural noise landscape simulation using Raysim"
)
