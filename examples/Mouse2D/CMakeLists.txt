set(PROJECT_NAME "Mouse2D")

add_executable(${PROJECT_NAME}
    main.cpp
    example_layers.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE Raysim::Raysim)

rs_enable_sanitizers(${PROJECT_NAME})

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "examples/${PROJECT_NAME}")
