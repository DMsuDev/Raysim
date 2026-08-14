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

rs_set_compiler_options(${PROJECT_NAME})
sanitizers_enable(${PROJECT_NAME})

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "examples/${PROJECT_NAME}")
