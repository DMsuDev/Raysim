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

  # Shorthand generator expressions used below
  set(_is_msvc   "$<CXX_COMPILER_ID:MSVC>")
  set(_is_gnu    "$<CXX_COMPILER_ID:GNU>")
  set(_is_clang  "$<CXX_COMPILER_ID:Clang,AppleClang>")
  set(_is_gcc_clang "$<CXX_COMPILER_ID:GNU,Clang,AppleClang>")

  set(_dbg   "$<CONFIG:Debug>")
  set(_rel   "$<CONFIG:Release>")
  set(_rwdi  "$<CONFIG:RelWithDebInfo>")

  # -----------------------------------------------------------------------
  # Compile options
  # -----------------------------------------------------------------------

  target_compile_options("${target}" PRIVATE
    # --- MSVC ---
    $<$<AND:${_dbg},${_is_msvc}>:   /Od /Zi /Ob0 /RTC1>
    $<$<AND:${_rel},${_is_msvc}>:   /O2 /Ob2 /GL>
    $<$<AND:${_rwdi},${_is_msvc}>:  /O2 /Zi  /Ob1>

    # --- GCC / Clang ---
    $<$<AND:${_dbg},${_is_gcc_clang}>:  -O0 -g3 -fno-omit-frame-pointer>
    $<$<AND:${_rel},${_is_gcc_clang}>:  -O3 -ffunction-sections -fdata-sections>
    $<$<AND:${_rwdi},${_is_gcc_clang}>: -O2 -g>
  )

  # -----------------------------------------------------------------------
  # Linker flags  (dead-strip in Release)
  # -----------------------------------------------------------------------

  target_link_options("${target}" PRIVATE
    # MSVC
    $<$<AND:${_rel},${_is_msvc}>: /LTCG>

    # Clang on Windows (lld-link frontend via clang++)
    $<$<AND:${_rel},$<PLATFORM_ID:Windows>,$<CXX_COMPILER_ID:Clang>>: -flto>

    # GCC on Linux
    $<$<AND:${_rel},$<CXX_COMPILER_ID:GNU>>: -Wl,--gc-sections>

    # Clang on Linux
    $<$<AND:${_rel},$<PLATFORM_ID:Linux>,$<CXX_COMPILER_ID:Clang>>: -Wl,--gc-sections>

    # AppleClang / Clang on macOS
    $<$<AND:${_rel},$<PLATFORM_ID:Darwin>,$<CXX_COMPILER_ID:Clang,AppleClang>>: -Wl,-dead_strip>
  )

  # -----------------------------------------------------------------------
  # Preprocessor definitions
  # -----------------------------------------------------------------------

  target_compile_definitions("${target}" PRIVATE
    $<${_dbg}:  _DEBUG>
    $<${_rel}:  NDEBUG>
    $<${_rwdi}: NDEBUG>
  )

endfunction()
