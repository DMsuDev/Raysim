set(PROJECT_NAME "BouncingBalls")

add_executable(${PROJECT_NAME}
    main.cpp
    example_ball.cpp
    example_helpers.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE Raysim::Raysim)

sanitizers_enable(${PROJECT_NAME})

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "examples/${PROJECT_NAME}")
