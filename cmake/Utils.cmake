# Provides utility functions for the CMake build system.
include_guard()

# ============================================================
# Helper: disable warnings for external libs
# ============================================================
function(rs_disable_warnings target)
    if(MSVC)
        target_compile_options(${target} PRIVATE /w)
    else()
        target_compile_options(${target} PRIVATE -w)
    endif()
endfunction()

function(rs_third_party_setup target)
    # Disable warnings for the target
    rs_disable_warnings(${target})

    # Group the target under "third_party" in IDEs
    set_target_properties(${target} PROPERTIES
        FOLDER "third_party"
    )
endfunction()
