set(PROJECT_NAME "NoiseLandscape")
add_executable(${PROJECT_NAME} main.cpp SkyLayer.cpp TerrainLayer.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE Raysim)
set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Examples/${PROJECT_NAME}")
