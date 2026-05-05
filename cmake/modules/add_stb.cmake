include_guard()

# Path to stb
set(STB_DIR ${CMAKE_SOURCE_DIR}/third_party/stb)

add_library(stb INTERFACE)

target_include_directories(stb
    SYSTEM INTERFACE
    ${STB_DIR}
)

# Group in IDEs
set_target_properties(stb PROPERTIES FOLDER "third_party")
