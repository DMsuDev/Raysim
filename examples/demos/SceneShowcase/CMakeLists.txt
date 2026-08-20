# ==============================================================================
#  RAYSIM EXAMPLES - Scene Showcase
# ============================================================================
#  Copyright (c) 2026 @DMsuDev. Licensed under the Apache License, Version 2.0.
#  See LICENSE file in the project root for full license text.
# ==============================================================================

set(PROJECT_NAME "SceneShowcase")

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE Raysim::Raysim)

rs_example_setup(SceneShowcase
  NAME        "Scene Showcase"
  DESCRIPTION "Scene showcase example using Raysim"
)
