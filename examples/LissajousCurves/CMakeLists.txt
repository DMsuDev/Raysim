set(PROJECT_NAME "LissajousCurves")

add_executable(${PROJECT_NAME}
    main.cpp
    LissajousSimulation.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE Raysim::Raysim)

rs_enable_sanitizers(${PROJECT_NAME})

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "examples/${PROJECT_NAME}")
