# ================================================
# C++ Sanitizers configuration
# ================================================
#
# Supported matrix:
#
#   Toolchain              | ASan | UBSan | TSan | MSan | LSan
#   -----------------------|------|-------|------|------|------
#   GCC / Linux            |  yes |  yes  |  yes |  no  |  yes
#   Clang / Linux          |  yes |  yes  |  yes | (1)  |  yes
#   Clang / Windows (GNU)  |  yes |  yes  |  no  |  no  |  no
#   MSVC                   |  no  |  no   |  no  |  no  |  no
#   MinGW                  |  --  |  --   |  --  |  --  |  --
#
#   (1) MSan on Clang/Linux requires a fully instrumented LLVM environment:
#       libc++ and all dependencies must be built with MSan.
#
#   clang-cl:
#       Sanitizers require linking against LLVM/compiler-rt instead of the
#       default MSVC CRT. This is intentionally unsupported in this project.
#
# Incompatible combinations:
#
#   - ASan + TSan
#   - MSan + ANY other sanitizer
#
# Notes:
#
#   - Sanitizers are applied only to non-Release configurations.
#   - Uses target-scoped flags only (no global CMAKE_CXX_FLAGS).
#   - Designed for Windows + Linux only.
# ================================================

include_guard()

# -----------------------------------------------------------------------------
# Internal helper:
# TRUE for every config except Release and MinSizeRel.
# Supports both single-config and multi-config generators.
# -----------------------------------------------------------------------------
function(_rs_san_config_genex out_var)
  set(${out_var} "$<NOT:$<CONFIG:Release,MinSizeRel>>" PARENT_SCOPE)
endfunction()

function(rs_enable_sanitizers target_name)

  # -------------------------------------------------------------------
  # Validation and early exits
  # (before any platform-specific logic or flag generation)
  # -------------------------------------------------------------------

  if(NOT TARGET ${target_name})
    message(FATAL_ERROR "[Sanitizers] Target '${target_name}' does not exist")
  endif()

  if(NOT RS_ENABLE_SANITIZERS)
    message(STATUS "[Sanitizers] Sanitizers are globally disabled -> skipping target '${target_name}'.")
    return()
  endif()

  if(NOT CMAKE_CONFIGURATION_TYPES)
    if(CMAKE_BUILD_TYPE MATCHES "^(Release|MinSizeRel)$")
      message(WARNING
        "[Sanitizers] Active build type is '${CMAKE_BUILD_TYPE}'. "
        "Sanitizers are applied only to Debug and RelWithDebInfo.")
    endif()
  endif()

  # -------------------------------------------------------------------------
  # Toolchain detection
  # -------------------------------------------------------------------------
  set(_is_gcc      FALSE)
  set(_is_clang    FALSE)
  set(_is_clang_cl FALSE)
  set(_is_msvc     FALSE)

  if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(_is_gcc TRUE)
  elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(_is_clang TRUE)
    if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC" OR
       CMAKE_CXX_SIMULATE_ID STREQUAL "MSVC")
      set(_is_clang_cl TRUE)
    endif()
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(_is_msvc TRUE)
  endif()

  # -------------------------------------------------------------------------
  # Hard exclusions: toolchains / environments with no reliable sanitizer support
  # -------------------------------------------------------------------------

  if(MINGW)
    message(WARNING
      "[Sanitizers] MinGW sanitizer support is inconsistent across toolchains "
      "and distributions. Sanitizers are disabled by default on MinGW.")
    return()
  endif()

  if(_is_msvc)
    message(WARNING
      "[Sanitizers] MSVC does not support sanitizers in this project.")
    return()
  endif()

  if(_is_clang_cl)
    message(WARNING
      "[Sanitizers] clang-cl sanitizers are intentionally unsupported "
      "in this project due to LLVM/compiler-rt runtime requirements.")
    return()
  endif()

  # -------------------------------------------------------------------------
  # Global incompatibility checks (before per-platform filtering)
  # -------------------------------------------------------------------------

  if(RS_ENABLE_ASAN AND RS_ENABLE_TSAN)
    message(FATAL_ERROR
      "[Sanitizers] ASan and TSan cannot be enabled at the same time.")
  endif()

  if(RS_ENABLE_MSAN AND
     (RS_ENABLE_ASAN OR RS_ENABLE_UBSAN OR RS_ENABLE_TSAN OR RS_ENABLE_LSAN))
    message(FATAL_ERROR
      "[Sanitizers] MSan must not be combined with any other sanitizer.")
  endif()

  # -------------------------------------------------------------------------
  # Capability matrix
  # -------------------------------------------------------------------------

  set(_allow_asan  FALSE)
  set(_allow_ubsan FALSE)
  set(_allow_tsan  FALSE)
  set(_allow_msan  FALSE)
  set(_allow_lsan  FALSE)

  # ---------------------------------------------------------
  # GCC / Linux
  # ---------------------------------------------------------

  if(_is_gcc AND CMAKE_SYSTEM_NAME STREQUAL "Linux")
    # GCC on Linux: ASan, UBSan, TSan, LSan. MSan requires compiler-rt.
    set(_allow_asan  TRUE)
    set(_allow_ubsan TRUE)
    set(_allow_tsan  TRUE)
    set(_allow_lsan  TRUE)

  # ---------------------------------------------------------
  # Clang / Linux
  # ---------------------------------------------------------

  elseif(_is_clang AND NOT _is_clang_cl AND CMAKE_SYSTEM_NAME STREQUAL "Linux")

    set(_allow_asan  TRUE)
    set(_allow_ubsan TRUE)
    set(_allow_tsan  TRUE)
    set(_allow_msan  TRUE)
    set(_allow_lsan  TRUE)

    if(RS_ENABLE_MSAN)
      message(STATUS
        "[Sanitizers] Note: MSan requires a fully instrumented LLVM environment. "
        "libc++ and all dependencies must be built with MSan instrumentation. "
        "See https://clang.llvm.org/docs/MemorySanitizer.html for setup details.")
    endif()

  # ---------------------------------------------------------
  # Clang / Windows (clang-cl)
  # ---------------------------------------------------------

  elseif(_is_clang AND WIN32)

    # Windows Clang GNU-style support is partial.
    # Only ASan and UBSan are considered reasonably usable.

    set(_allow_asan  TRUE)
    set(_allow_ubsan TRUE)

  else()
    message(WARNING
      "[Sanitizers] Unrecognised compiler/platform combination "
      "(compiler: ${CMAKE_CXX_COMPILER_ID}, "
      "Sanitizers are disabled.")
    return()
  endif()

  # -------------------------------------------------------------------------
  # Warn about requested sanitizers unsupported on this platform
  # -------------------------------------------------------------------------
  if(RS_ENABLE_ASAN AND NOT _allow_asan)
    message(WARNING
      "[Sanitizers] ASan is not supported on this platform/toolchain -> skipped.")
  endif()
  if(RS_ENABLE_MSAN AND NOT _allow_msan)
    message(WARNING
      "[Sanitizers] MSan is not supported on this platform/toolchain -> skipped.")
  endif()
  if(RS_ENABLE_TSAN AND NOT _allow_tsan)
    message(WARNING
      "[Sanitizers] TSan is not supported on this platform/toolchain -> skipped.")
  endif()
  if(RS_ENABLE_UBSAN AND NOT _allow_ubsan)
    message(WARNING
      "[Sanitizers] UBSan is not supported on this platform/toolchain -> skipped.")
  endif()
  if(RS_ENABLE_LSAN AND NOT _allow_lsan)
    message(WARNING
      "[Sanitizers] LSan is not supported on this platform/toolchain -> skipped.")
  endif()

  # -------------------------------------------------------------------------
  # Build the final sanitizer list (requested AND supported)
  # -------------------------------------------------------------------------
  set(_sanitizers)

  if(RS_ENABLE_ASAN AND _allow_asan)
    list(APPEND _sanitizers address)
  endif()

  if(RS_ENABLE_UBSAN AND _allow_ubsan)
    list(APPEND _sanitizers undefined)
  endif()

  if(RS_ENABLE_TSAN AND _allow_tsan)
    list(APPEND _sanitizers thread)
  endif()

  if(RS_ENABLE_MSAN AND _allow_msan)
    list(APPEND _sanitizers memory)
  endif()

  if(RS_ENABLE_LSAN AND _allow_lsan)
    if(RS_ENABLE_ASAN)
      message(WARNING
        "[Sanitizers] LSan is redundant when ASan is active "
        "(ASan already includes leak detection) -> skipped.")
    else()
      list(APPEND _sanitizers leak)
    endif()
  endif()

  if(NOT _sanitizers)
    message(STATUS
      "[Sanitizers] No sanitizers were enabled for target '${target_name}'.")
    return()
  endif()

  # -------------------------------------------------------------------------
  # Build flags
  # -------------------------------------------------------------------------

  list(JOIN _sanitizers "," _san_list)

  set(_compile_flags
    "-fsanitize=${_san_list}"
    "-fno-omit-frame-pointer"
  )

  set(_link_flags
    "-fsanitize=${_san_list}"
  )

  # -------------------------------------------------------------------------
  # Apply flags (scoped to the target, respecting build config)
  # -------------------------------------------------------------------------

  _rs_san_config_genex(_cfg_genex)

  message(STATUS
    "[Sanitizers] Enabling '${_san_list}' for target '${target_name}'")

  # GCC / plain Clang: GCC-compatible flags.
  target_compile_options(${target_name} PRIVATE
    $<${_cfg_genex}:${_compile_flags}>
  )

  target_link_options(${target_name} PRIVATE
    $<${_cfg_genex}:${_link_flags}>
  )

endfunction()
