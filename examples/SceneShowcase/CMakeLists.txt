set(PROJECT_NAME "SceneShowcase")

add_executable(${PROJECT_NAME}
    main.cpp
    StarField.cpp
    PlasmaArt.cpp
    ClockMandala.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE Raysim::Raysim)

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "examples/${PROJECT_NAME}")

add_dependencies(${PROJECT_NAME} SharedAssets)
