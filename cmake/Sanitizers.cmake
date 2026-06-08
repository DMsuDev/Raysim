# ================================================
# C++ Sanitizers configuration
# ================================================
#
# SUPPORTED MATRIX
# ----------------
#   Toolchain              | ASan | UBSan | TSan | MSan | LSan
#   -----------------------|------|-------|------|------|------
#   GCC / Linux            |  yes |  yes  |  yes |  no  |  yes
#   Clang / Linux          |  yes |  yes  |  yes | (1)  |  yes
#   Clang / Windows (GNU)  |  yes |  no   |  no  |  no  |  no
#   MSVC                   |  no  |  no   |  no  |  no  |  no
#   clang-cl               |  no  |  no   |  no  |  no  |  no
#   MinGW                  |  --  |  --   |  --  |  --  |  --
#
#   (1) MSan on Clang/Linux requires a fully instrumented LLVM environment:
#       libc++ and all dependencies must be built with MSan instrumentation.
#       See https://clang.llvm.org/docs/MemorySanitizer.html
#
# INCOMPATIBLE COMBINATIONS
# -------------------------
#   - ASan  + TSan
#   - MSan  + any other sanitizer
#
# NOTES
# -----
#   - Sanitizers are applied only to Debug / RelWithDebInfo configurations.
#     Release and MinSizeRel are always skipped (via generator expressions in
#     multi-config generators, or a warning in single-config generators).
#   - Flags are scoped to the target (PRIVATE). No global CMAKE_CXX_FLAGS
#     pollution.
#   - On Windows + Clang/GNU, the ASan runtime DLL is copied next to the
#     executable automatically. If the DLL is not found, linking falls back
#     to -static-libsan.
# ================================================

include_guard()

# -----------------------------------------------------------------------------
# Internal helper:
# TRUE for every config except Release and MinSizeRel.
# Supports both single-config and multi-config generators.
# -----------------------------------------------------------------------------
function(_san_config_genex out_var)
  set(${out_var} "$<NOT:$<CONFIG:Release,MinSizeRel>>" PARENT_SCOPE)
endfunction()

# -----------------------------------------------------------------------
# Internal helper: apply a list of flags to a target under a genex guard
# -----------------------------------------------------------------------
function(_san_apply_flags target_name cfg_genex compile_flags link_flags)

  foreach(_flag IN LISTS compile_flags)
    target_compile_options(${target_name} PRIVATE $<${cfg_genex}:${_flag}>)
  endforeach()

  foreach(_flag IN LISTS link_flags)
    target_link_options(${target_name} PRIVATE $<${cfg_genex}:${_flag}>)
  endforeach()

endfunction()

# -----------------------------------------------------------------------
# Public API
# -----------------------------------------------------------------------
function(sanitizers_enable target_name)

  # --- target must exist ------------------------------------------------
  if(NOT TARGET ${target_name})
    message(FATAL_ERROR "[Sanitizers] Target '${target_name}' does not exist.")
  endif()

  # --- bail out if nothing is requested ---------------------------------
  if(NOT RS_SANITIZE_ASAN  AND NOT RS_SANITIZE_UBSAN AND
     NOT RS_SANITIZE_TSAN  AND NOT RS_SANITIZE_MSAN  AND NOT RS_SANITIZE_LSAN)
    message(STATUS "[Sanitizers] All sanitizers OFF -> skipping '${target_name}'.")
    return()
  endif()

  # --- warn once when sanitizers are ineffective in Release/MinSizeRel ---------

  # Warn only once per configure
  if(NOT _SAN_WARNED_RELEASE_BUILD)
    if(NOT CMAKE_CONFIGURATION_TYPES)

      # Single-config generators (Make, Ninja)
      if(CMAKE_BUILD_TYPE MATCHES "^(Release|MinSizeRel)$")
        message(WARNING
          "[Sanitizers] Active build type is '${CMAKE_BUILD_TYPE}'. "
          "Sanitizers apply only to Debug and RelWithDebInfo.")
        set(_SAN_WARNED_RELEASE_BUILD TRUE CACHE INTERNAL "")
      endif()

    else()

      # Multi-config generators (VS, Xcode, Ninja Multi-Config)
      message(STATUS
        "[Sanitizers] Multi-config generator detected. "
        "Sanitizers will be disabled automatically for Release/MinSizeRel "
        "via generator expressions.")
      set(_SAN_WARNED_RELEASE_BUILD TRUE CACHE INTERNAL "")

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
    if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
      set(_is_clang_cl TRUE)
    endif()
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(_is_msvc TRUE)
  endif()

  # -------------------------------------------------------------------------
  # Hard exclusions: toolchains / environments with no reliable sanitizer support
  # -------------------------------------------------------------------------

  if(MINGW)
    if(NOT _SAN_WARNED_MINGW)
      message(WARNING
        "[Sanitizers] MinGW sanitizer support is inconsistent. Skipping '${target_name}'.")
      set(_SAN_WARNED_MINGW TRUE CACHE INTERNAL "")
    endif()
    return()
  endif()

  if(_is_msvc)
    if(NOT _SAN_WARNED_MSVC)
      message(WARNING "[Sanitizers] MSVC does not support sanitizers. Skipping.")
      set(_SAN_WARNED_MSVC TRUE CACHE INTERNAL "")
    endif()
    return()
  endif()

  if(_is_clang_cl)
    if(NOT _SAN_WARNED_CLANG_CL)
      message(WARNING
        "[Sanitizers] clang-cl sanitizers are unsupported (requires LLVM/compiler-rt CRT). "
        "Skipping '${target_name}'.")
      set(_SAN_WARNED_CLANG_CL TRUE CACHE INTERNAL "")
    endif()
    return()
  endif()

  # -------------------------------------------------------------------------
  # Global incompatibility checks (before per-platform filtering)
  # -------------------------------------------------------------------------

  if(RS_SANITIZE_ASAN AND RS_SANITIZE_TSAN)
    message(FATAL_ERROR "[Sanitizers] ASan and TSan cannot be enabled simultaneously.")
  endif()

  set(_other_san FALSE)
  if(RS_SANITIZE_ASAN OR RS_SANITIZE_UBSAN OR RS_SANITIZE_TSAN OR RS_SANITIZE_LSAN)
    set(_other_san TRUE)
  endif()

  if(RS_SANITIZE_MSAN AND _other_san)
    message(FATAL_ERROR "[Sanitizers] MSan cannot be combined with any other sanitizer.")
  endif()

  # -------------------------------------------------------------------------
  # Capability matrix
  # -------------------------------------------------------------------------

  set(_allow_asan  FALSE)
  set(_allow_ubsan FALSE)
  set(_allow_tsan  FALSE)
  set(_allow_msan  FALSE)
  set(_allow_lsan  FALSE)

  # --- GCC / Linux -------------------------------------------------------

  if(_is_gcc AND CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(_allow_asan  TRUE)
    set(_allow_ubsan TRUE)
    set(_allow_tsan  TRUE)
    set(_allow_lsan  TRUE)

  # --- Clang / Linux -------------------------------------------------------

  elseif(_is_clang AND NOT _is_clang_cl AND CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(_allow_asan  TRUE)
    set(_allow_ubsan TRUE)
    set(_allow_tsan  TRUE)
    set(_allow_msan  TRUE)
    set(_allow_lsan  TRUE)

    if(RS_SANITIZE_MSAN)
      message(STATUS
        "[Sanitizers] MSan requires a fully instrumented LLVM environment. "
        "See https://clang.llvm.org/docs/MemorySanitizer.html")
    endif()

  # --- Clang / Windows (GNU frontend) --------------------------------------

  # Windows Clang GNU-style support is partial.
  # Only ASan and UBSan are considered reasonably usable.
  elseif(_is_clang AND WIN32)
    set(_allow_asan  TRUE)
    set(_allow_ubsan FALSE)

  else()
    if(NOT _SAN_WARNED_UNKNOWN)
      message(WARNING
        "[Sanitizers] Unrecognised compiler/platform "
        "(${CMAKE_CXX_COMPILER_ID} / ${CMAKE_SYSTEM_NAME}). Sanitizers disabled.")
      set(_SAN_WARNED_UNKNOWN TRUE CACHE INTERNAL "")
    endif()
    return()
  endif()

  # --- warn about unsupported requests ----------------------------------

  foreach(_san IN ITEMS ASAN UBSAN TSAN MSAN LSAN)
    if(SANITIZE_${_san} AND NOT _allow_${_san} AND NOT _SAN_WARNED_${_san}_UNSUPPORTED)  # cmake-lint: disable=E1120
      string(TOLOWER "${_san}" _san_lower)
      message(WARNING
        "[Sanitizers] ${_san} is not supported on this platform/toolchain -> skipped.")
      set(_SAN_WARNED_${_san}_UNSUPPORTED TRUE CACHE INTERNAL "")
    endif()
  endforeach()

  # -------------------------------------------------------------------------
  # Build the final sanitizer list (requested AND supported)
  # -------------------------------------------------------------------------
  set(_sanitizers)

  if(RS_SANITIZE_ASAN AND _allow_asan)
    list(APPEND _sanitizers address)
  endif()

  if(RS_SANITIZE_UBSAN AND _allow_ubsan)
    list(APPEND _sanitizers undefined)
  endif()

  if(RS_SANITIZE_TSAN AND _allow_tsan)
    list(APPEND _sanitizers thread)
  endif()

  if(RS_SANITIZE_MSAN AND _allow_msan)
    list(APPEND _sanitizers memory)
  endif()

  if(RS_SANITIZE_LSAN AND _allow_lsan)
    if(RS_SANITIZE_ASAN)
      if(NOT _SAN_WARNED_LSAN_REDUNDANT)
        message(WARNING
          "[Sanitizers] LSan is redundant when ASan is active (ASan includes leak detection). "
          "Skipping LSan.")
        set(_SAN_WARNED_LSAN_REDUNDANT TRUE CACHE INTERNAL "")
      endif()
    else()
      list(APPEND _sanitizers leak)
    endif()
  endif()

  if(NOT _sanitizers)
    message(STATUS "[Sanitizers] No sanitizers active for '${target_name}'.")
    return()
  endif()

  list(JOIN _sanitizers "," _san_list)

  # --- Windows/Clang: locate ASan DLL before building flags -------------
  set(_extra_compile_flags)
  set(_extra_link_flags)

  if(RS_SANITIZE_ASAN AND _allow_asan AND WIN32)
    execute_process(
      COMMAND ${CMAKE_CXX_COMPILER} --print-runtime-dir
      OUTPUT_VARIABLE _clang_rt_dir
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    get_filename_component(_clang_rt_parent "${_clang_rt_dir}" DIRECTORY)
    set(_search_dirs
      "${_clang_rt_dir}"
      "${_clang_rt_parent}/x86_64-pc-windows-msvc"
      "${_clang_rt_parent}/x86_64-unknown-windows-msvc"
      "${_clang_rt_parent}/x86_64-unknown-windows-gnu"
      "${_clang_rt_parent}/x86_64-w64-windows-gnu"
    )

    set(_asan_dlls)
    foreach(_dir IN LISTS _search_dirs)
      file(GLOB _found
        "${_dir}/clang_rt.asan_dynamic-x86_64.dll"
        "${_dir}/clang_rt.asan*.dll"
      )
      if(_found)
        list(APPEND _asan_dlls ${_found})
        break()
      endif()
    endforeach()

    if(NOT _asan_dlls)
      if(NOT _SAN_WARNED_ASAN_DLL_MISSING)
        message(WARNING
          "[Sanitizers] ASan runtime DLL not found. "
          "ASan will be disabled for '${target_name}'.\n"
          "To fix: ensure clang_rt.asan_dynamic-x86_64.dll is accessible.\n"
          "Searched: ${_search_dirs}")
        set(_SAN_WARNED_ASAN_DLL_MISSING TRUE CACHE INTERNAL "")
      endif()
      list(REMOVE_ITEM _sanitizers address)
      list(JOIN _sanitizers "," _san_list)
      if(NOT _san_list)
        return()
      endif()
    endif()

  endif()

  # --- assemble flags ---------------------------------------------------
  set(_compile_flags
    "-fsanitize=${_san_list}"
    "-fno-omit-frame-pointer"
    ${_extra_compile_flags}
  )
  set(_link_flags
    "-fsanitize=${_san_list}"
    ${_extra_link_flags}
  )

  # --- apply flags ------------------------------------------------------
  _san_config_genex(_cfg_genex)
  _san_apply_flags(${target_name} "${_cfg_genex}" "${_compile_flags}" "${_link_flags}")

  message(STATUS "[Sanitizers] '${_san_list}' enabled for target '${target_name}'")

  # --- post-build: copy ASan DLLs if found ------------------------------
  if(DEFINED _asan_dlls AND _asan_dlls)
    foreach(_dll IN LISTS _asan_dlls)
      add_custom_command(TARGET ${target_name} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
          "${_dll}"
          "$<TARGET_FILE_DIR:${target_name}>"
        COMMENT "[Sanitizers] Copying ASan runtime: ${_dll}"
      )
    endforeach()
  endif()

endfunction()
