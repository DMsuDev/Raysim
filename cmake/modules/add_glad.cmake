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

rs_third_party_setup(glad)
