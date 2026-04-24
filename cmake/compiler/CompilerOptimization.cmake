# ================================================
# Compiler optimization levels
# ================================================
include_guard()

function(rs_enable_optimizations target)
    if(NOT TARGET ${target})
        message(FATAL_ERROR "Target '${target}' does not exist")
    endif()

    # Compile options
    target_compile_options(${target} PRIVATE

        # MSVC
        $<$<AND:$<CONFIG:Debug>,$<CXX_COMPILER_ID:MSVC>>:/Od /Zi /Ob0>
        $<$<AND:$<CONFIG:Release>,$<CXX_COMPILER_ID:MSVC>>:/O2 /Ob2>
        $<$<AND:$<CONFIG:RelWithDebInfo>,$<CXX_COMPILER_ID:MSVC>>:/O2 /Zi>

        # GCC / Clang
        $<$<AND:$<CONFIG:Debug>,$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>>:-O0 -g3 -fno-omit-frame-pointer>
        $<$<AND:$<CONFIG:Release>,$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>>:-O3>
        $<$<AND:$<CONFIG:RelWithDebInfo>,$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>>:-O2 -g>
    )

    # Defines for NDEBUG in Release and RelWithDebInfo
    target_compile_definitions(${target} PRIVATE
        $<$<CONFIG:Release,RelWithDebInfo>:NDEBUG>
    )

    message(STATUS "Optimizations enabled for ${target}")

endfunction()
