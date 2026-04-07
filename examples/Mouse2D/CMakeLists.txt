set(PROJECT_NAME "Mouse2D")

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE Raysim::Raysim)

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "examples/${PROJECT_NAME}")

add_dependencies(${PROJECT_NAME} SharedAssets)
