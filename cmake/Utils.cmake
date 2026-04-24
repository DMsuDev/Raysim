# Provides utility functions for the CMake build system.
include_guard()

# ============================================================
# Helper: disable warnings for external libs
# ============================================================
function(disable_warnings target)
    get_target_property(target_type ${target} TYPE)

    if (target_type STREQUAL "INTERFACE_LIBRARY")
        if (MSVC)
            target_compile_options(${target} INTERFACE /W0)
        else()
            target_compile_options(${target} INTERFACE -w)
        endif()
    else()
        if (MSVC)
            target_compile_options(${target} PRIVATE /W0)
        else()
            target_compile_options(${target} PRIVATE -w)
        endif()
    endif()
endfunction()
