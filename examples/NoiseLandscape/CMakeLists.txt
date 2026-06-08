set(PROJECT_NAME "NoiseLandscape")

add_executable(${PROJECT_NAME} main.cpp SkyLayer.cpp TerrainLayer.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE Raysim::Raysim)

sanitizers_enable(${PROJECT_NAME})

set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "examples/${PROJECT_NAME}")
