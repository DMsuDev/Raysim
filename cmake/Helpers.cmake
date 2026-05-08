# ===========================================================================
# Helper functions for third-party integration
# ===========================================================================

include_guard()

function(rs_third_party_setup target)
    if(MSVC)
        # /W0 disables all warnings; also suppress any previously set /W level to avoid D9025
        target_compile_options(${target} PRIVATE /W0)
    else()
        target_compile_options(${target} PRIVATE -w)
    endif()
    set_target_properties(${target} PROPERTIES FOLDER "third_party")
endfunction()
