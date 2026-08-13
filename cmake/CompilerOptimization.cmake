# ==============================================================================
#  RAYSIM - OPTIMIZATIONS
# ==============================================================================
#  Description: Configures per-configuration optimization and dead-code
#               elimination flags for CMake targets.
#
#  Copyright (c) 2026 @DMsuDev. Licensed under the Apache License, Version 2.0.
#  See LICENSE file in the project root for full license text.
# ==============================================================================

include_guard()

# ------------------------------------------------------------------------------
#  rs_enable_optimizations(<target>)
# ------------------------------------------------------------------------------
#  Applies Raysim's optimization policy to a single target with PRIVATE scope.
#  Call once per library or executable target after it is declared.
# ------------------------------------------------------------------------------
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

  # ----------------------------------------------------------------------------
  # Compile options
  # ----------------------------------------------------------------------------

  target_compile_options("${target}" PRIVATE
    # MSVC: function-level linking in Release; enables safer /OPT:REF at link time
    $<$<AND:${_rel},${_is_msvc}>:/Gy>

    # GCC/Clang: frame pointer in Debug for profiler/sanitizer compatibility
    $<$<AND:${_dbg},${_is_gcc_clang}>:-fno-omit-frame-pointer>

    # GCC/Clang: section splitting in Release/RelWithDebInfo enables dead-strip
    $<$<AND:$<OR:${_rel},${_rwdi}>,${_is_gcc_clang}>:-ffunction-sections>
    $<$<AND:$<OR:${_rel},${_rwdi}>,${_is_gcc_clang}>:-fdata-sections>
  )

  # ----------------------------------------------------------------------------
  # Linker flags (dead-strip in Release and RelWithDebInfo)
  # ----------------------------------------------------------------------------

  target_link_options("${target}" PRIVATE
    # Linux: GCC / Clang
    $<$<AND:$<OR:${_rel},${_rwdi}>,$<PLATFORM_ID:Linux>,$<CXX_COMPILER_ID:GNU,Clang>>:-Wl,--gc-sections>

    # macOS: Clang / AppleClang
    $<$<AND:$<OR:${_rel},${_rwdi}>,$<PLATFORM_ID:Darwin>,$<CXX_COMPILER_ID:Clang,AppleClang>>:-Wl,-dead_strip>
  )

endfunction()
