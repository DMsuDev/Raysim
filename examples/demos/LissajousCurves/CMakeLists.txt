# ==============================================================================
#  RAYSIM EXAMPLES - Lissajous Curves
# ============================================================================
#  Copyright (c) 2026 @DMsuDev. Licensed under the Apache License, Version 2.0.
#  See LICENSE file in the project root for full license text.
# ==============================================================================

set(PROJECT_NAME "LissajousCurves")

add_executable(${PROJECT_NAME}
  main.cpp
  LissajousSimulation.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE Raysim::Raysim)

rs_example_setup(LissajousCurves
  NAME        "Lissajous Curves"
  DESCRIPTION "Parametric Lissajous curve simulation using Raysim"
)
