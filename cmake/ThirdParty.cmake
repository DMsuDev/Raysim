include_guard()

# ===========================================================================
# Third Party Libraries
# ===========================================================================

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)

# System threads
find_package(Threads REQUIRED)

# spdlog (header-only)
set(SPDLOG_DIR ${CMAKE_SOURCE_DIR}/third_party/spdlog)
add_library(spdlog INTERFACE)
target_include_directories(spdlog SYSTEM INTERFACE ${SPDLOG_DIR}/include)
target_link_libraries(spdlog INTERFACE Threads::Threads)
set_target_properties(spdlog PROPERTIES FOLDER "third_party")

# stb (header-only)
set(STB_DIR ${CMAKE_SOURCE_DIR}/third_party/stb)
add_library(stb INTERFACE)
target_include_directories(stb SYSTEM INTERFACE ${STB_DIR})
set_target_properties(stb PROPERTIES FOLDER "third_party")

# ImGui core + backend
include(add_imgui)

# Backend (windowing + graphics + imgui binding)
include(${CMAKE_SOURCE_DIR}/cmake/Backends/BackendRegistry.cmake)
