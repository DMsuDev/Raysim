# ================================================
# Compiler optimization levels by build type
# ================================================
include_guard()

function(set_project_optimization target)
    if(NOT TARGET ${target})
        message(FATAL_ERROR "Target '${target}' does not exist")
    endif()

    # Debug configuration
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        if(MSVC)
            target_compile_options(${target} PRIVATE
                /Od        # Disable optimizations
                /Zi        # Full debug info
                /Ob0       # No inline
            )
        elseif(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang" OR
               CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            target_compile_options(${target} PRIVATE
                -O0        # No optimization
                -g3        # Max debug info
                -fno-omit-frame-pointer
            )
        endif()

    # Release configuration
    elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
        if(MSVC)
            target_compile_options(${target} PRIVATE
                /O2        # Optimize for speed
                /Ob2       # Aggressive inlining
                /DNDEBUG
            )
        elseif(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang" OR
               CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            target_compile_options(${target} PRIVATE
                -O3        # Max optimization
                -DNDEBUG
            )
        endif()

    # RelWithDebInfo configuration
    elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
        if(MSVC)
            target_compile_options(${target} PRIVATE
                /O2
                /Zi
                /DNDEBUG
            )
        elseif(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang" OR
               CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            target_compile_options(${target} PRIVATE
                -O2
                -g
                -DNDEBUG
            )
        endif()

    endif()
endfunction()
