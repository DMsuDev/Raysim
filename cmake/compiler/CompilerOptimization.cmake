# ===========================================================================
# Provides rs_enable_optimizations(<target>) which sets per-configuration
# optimisation flags on a first-party target.
#
# Flags are applied PRIVATE so they never leak to dependents.
#
# Usage:
#   rs_enable_optimizations(my_target)
# ===========================================================================

include_guard()

function(rs_enable_optimizations target)
  if(NOT TARGET "${target}")
    message(FATAL_ERROR
      "[rs] rs_enable_optimizations: target '${target}' does not exist.")
  endif()

  set(_is_msvc      "$<CXX_COMPILER_ID:MSVC>")
  set(_is_gcc_clang "$<CXX_COMPILER_ID:GNU,Clang,AppleClang>")

  set(_dbg  "$<CONFIG:Debug>")
  set(_rel  "$<CONFIG:Release>")
  set(_rwdi "$<CONFIG:RelWithDebInfo>")

  # -----------------------------------------------------------------------
  # Compile options
  # -----------------------------------------------------------------------

  target_compile_options("${target}" PRIVATE
    # MSVC: /MP (parallel compilation) not set by CMake
    $<${_is_msvc}:/MP>

    # GCC/Clang: frame pointer in Debug for profiler/sanitizer compatibility
    $<$<AND:${_dbg},${_is_gcc_clang}>:-fno-omit-frame-pointer>

    # GCC/Clang: section splitting in Release/RelWithDebInfo enables dead-strip
    $<$<AND:$<OR:${_rel},${_rwdi}>,${_is_gcc_clang}>:-ffunction-sections -fdata-sections>
  )

  # -----------------------------------------------------------------------
  # Linker flags  (dead-strip in Release and RelWithDebInfo)
  # -----------------------------------------------------------------------

  target_link_options("${target}" PRIVATE
    # Linux: GCC / Clang
    $<$<AND:$<OR:${_rel},${_rwdi}>,$<PLATFORM_ID:Linux>,$<CXX_COMPILER_ID:GNU,Clang>>:-Wl,--gc-sections>

    # macOS: Clang / AppleClang
    $<$<AND:$<OR:${_rel},${_rwdi}>,$<PLATFORM_ID:Darwin>,$<CXX_COMPILER_ID:Clang,AppleClang>>:-Wl,-dead_strip>
  )

endfunction()
