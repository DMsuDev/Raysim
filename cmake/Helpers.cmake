# ===========================================================================
# Helper functions for third-party integration
# ===========================================================================

include_guard()

function(rs_third_party_setup target)
    if(MSVC)
        target_compile_options(${target} PRIVATE /w)
    else()
        target_compile_options(${target} PRIVATE -w)
    endif()
    set_target_properties(${target} PROPERTIES FOLDER "third_party")
endfunction()
