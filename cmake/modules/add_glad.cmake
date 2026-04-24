include_guard()

# Paths to glad source and include directories
set(GLAD_DIR ${CMAKE_SOURCE_DIR}/third_party/glad)

# ============================================================
# GLAD (OpenGL loader)
# ============================================================

# Force C language for glad's gl.c to avoid C++ name mangling issues
set_source_files_properties(${GLAD_DIR}/src/gl.c PROPERTIES LANGUAGE C)

add_library(glad STATIC
    ${GLAD_DIR}/src/gl.c
)

target_include_directories(glad
    SYSTEM PUBLIC
    ${GLAD_DIR}/include
)

disable_warnings(glad)

# Group glad under "third_party" in IDEs
set_target_properties(glad PROPERTIES FOLDER "third_party")
