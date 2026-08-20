# ==============================================================================
#  RAYSIM EXAMPLES - Mouse 2D
# ============================================================================
#  Copyright (c) 2026 @DMsuDev. Licensed under the Apache License, Version 2.0.
#  See LICENSE file in the project root for full license text.
# ==============================================================================

set(PROJECT_NAME "Mouse2D")

add_executable(${PROJECT_NAME}
  main.cpp
  example_layers.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE Raysim::Raysim)

rs_example_setup(Mouse2D
  NAME        "Mouse 2D"
  DESCRIPTION "Mouse 2D interaction example using Raysim"
)
