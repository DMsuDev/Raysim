set(PROJECT_NAME "BouncingBalls")
add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE Raysim)

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Examples/${PROJECT_NAME}")
