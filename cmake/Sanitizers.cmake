# ============================================================================
#  RAYSIM - SANITIZERS CONFIGURATION MODULE
# ============================================================================
#  Description: Target-scoped runtime sanitizer setup for Clang and GCC.
#               Handles toolchain compatibility, target scoping, and Windows
#               dynamic runtime dependency copying.
#
#  Copyright (c) 2026 Dayron Mustelier (@DMsuDev)
#  Licensed under the Apache License, Version 2.0.
# ============================================================================

# ============================================================================
#  SUPPORTED MATRIX
#  ---------------------------------------------------------------------------
#   Toolchain                 | ASan | UBSan | TSan | MSan | LSan
#   --------------------------|------|-------|------|------|------
#   GCC / Linux               |  yes |  yes  |  yes |  no  |  yes
#   Clang / Linux             |  yes |  yes  |  yes | (1)  |  yes
#   Clang / Windows (non-cl)  |  yes |  no   |  no  |  no  |  no
#   MSVC                      |  (*) |  no   |  no  |  no  |  no
#   clang-cl                  |  no  |  no   |  no  |  no  |  no
#   MinGW                     |  --  |  --   |  --  |  --  |  --
#
#   (1) MSan requires a fully instrumented LLVM environment.
#       See https://clang.llvm.org/docs/MemorySanitizer.html
#
#   (*) MSVC ASan not yet implemented (requires /fsanitize=address backend).
#
#  NOTES
#  -----
#   - Incompatible combinations (ASan+TSan, MSan+any) are validated in the
#     root CMakeLists.txt before this module is included.
#   - Flags are scoped PRIVATE to the target. No global CMAKE_CXX_FLAGS pollution.
#   - Only applied to Debug and RelWithDebInfo via generator expressions.
#   - On Windows/Clang, the ASan runtime DLL is copied next to the executable.
#     Pass EXECUTABLE_TARGET to control which executable receives the DLL.
#     Call sanitizers_enable() with EXECUTABLE_TARGET only once per executable.
# ============================================================================

include_guard()

# ------------------------------------------------------------------------------
#  Public API
# ------------------------------------------------------------------------------
#  sanitizers_enable(<target> [EXECUTABLE_TARGET <exe_target>])
#
#  EXECUTABLE_TARGET: Executable that receives the ASan DLL (Windows/Clang).
#                     Defaults to <target>.
# ------------------------------------------------------------------------------
function(sanitizers_enable target_name)

  cmake_parse_arguments(PARSE_ARGV 1 _SAN "" "EXECUTABLE_TARGET" "")

  # --- Early exit: nothing requested -----------------------------------------
  set(_any_requested FALSE)
  foreach(_san IN ITEMS ASAN UBSAN TSAN MSAN LSAN)
    if(RS_SANITIZE_${_san})
      set(_any_requested TRUE)
      break()
    endif()
  endforeach()

  if(NOT _any_requested)
    return()
  endif()

  # --- Target validation -----------------------------------------------------
  if(NOT TARGET ${target_name})
    message(FATAL_ERROR "[Sanitizers] Target '${target_name}' does not exist.")
  endif()

  if(_SAN_EXECUTABLE_TARGET AND NOT TARGET ${_SAN_EXECUTABLE_TARGET})
    message(FATAL_ERROR
      "[Sanitizers] EXECUTABLE_TARGET '${_SAN_EXECUTABLE_TARGET}' does not exist.")
  endif()

  if(NOT _SAN_EXECUTABLE_TARGET)
    set(_SAN_EXECUTABLE_TARGET ${target_name})
  endif()

  # --- Toolchain detection ---------------------------------------------------
  set(_is_gcc      FALSE)
  set(_is_clang    FALSE)
  set(_is_clang_cl FALSE)

  if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(_is_gcc TRUE)
  elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(_is_clang TRUE)
    if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
      set(_is_clang_cl TRUE)
    endif()
  endif()

  # --- Hard exclusions (warn once per toolchain) -----------------------------
  if(MINGW)
    if(NOT _SAN_WARNED_MINGW)
      message(WARNING "[Sanitizers] MinGW sanitizer support is inconsistent. Skipping.")
      set(_SAN_WARNED_MINGW TRUE CACHE INTERNAL "")
    endif()
    return()
  endif()

  if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    if(NOT _SAN_WARNED_MSVC)
      message(WARNING
        "[Sanitizers] MSVC ASan (/fsanitize=address) not yet implemented. Skipping.")
      set(_SAN_WARNED_MSVC TRUE CACHE INTERNAL "")
    endif()
    return()
  endif()

  if(_is_clang_cl)
    if(NOT _SAN_WARNED_CLANG_CL)
      message(WARNING
        "[Sanitizers] clang-cl sanitizers unsupported (requires LLVM/compiler-rt CRT). Skipping.")
      set(_SAN_WARNED_CLANG_CL TRUE CACHE INTERNAL "")
    endif()
    return()
  endif()

  # --- Capability matrix -----------------------------------------------------
  set(_allow_asan  FALSE)
  set(_allow_ubsan FALSE)
  set(_allow_tsan  FALSE)
  set(_allow_msan  FALSE)
  set(_allow_lsan  FALSE)

  if(_is_gcc AND CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(_allow_asan TRUE)
    set(_allow_ubsan TRUE)
    set(_allow_tsan TRUE)
    set(_allow_lsan  TRUE)

  elseif(_is_clang AND NOT _is_clang_cl AND CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(_allow_asan TRUE)
    set(_allow_ubsan TRUE)
    set(_allow_tsan TRUE)
    set(_allow_msan  TRUE)
    set(_allow_lsan TRUE)

    if(RS_SANITIZE_MSAN)
      message(STATUS
        "[Sanitizers] MSan requires a fully instrumented LLVM environment. "
        "See https://clang.llvm.org/docs/MemorySanitizer.html")
    endif()

  elseif(_is_clang AND NOT _is_clang_cl AND WIN32)
    set(_allow_asan TRUE)

  else()
    if(NOT _SAN_WARNED_UNKNOWN)
      message(WARNING
        "[Sanitizers] Unrecognised compiler/platform "
        "(${CMAKE_CXX_COMPILER_ID} / ${CMAKE_SYSTEM_NAME}). Sanitizers disabled.")
      set(_SAN_WARNED_UNKNOWN TRUE CACHE INTERNAL "")
    endif()
    return()
  endif()

  # --- Warn about unsupported requested sanitizers
  foreach(_san IN ITEMS asan ubsan tsan msan lsan)
    string(TOUPPER "${_san}" _san_upper)
    if(RS_SANITIZE_${_san_upper} AND NOT _allow_${_san})
      set(_warn_var "_SAN_WARNED_${_san_upper}_UNSUPPORTED")
      if(NOT ${_warn_var})
        message(WARNING
          "[Sanitizers] ${_san_upper} is not supported on this platform/toolchain -> skipped.")
        set(${_warn_var} TRUE CACHE INTERNAL "")
      endif()
    endif()
  endforeach()

  # --- Build final sanitizer list (requested AND supported) ------------------
  set(_sanitizers)

  foreach(_san asan ubsan tsan msan)
    string(TOUPPER "${_san}" _san_upper)
    # Map CMake name -> sanitizer flag name
    set(_flag_asan  address)
    set(_flag_ubsan undefined)
    set(_flag_tsan  thread)
    set(_flag_msan  memory)
    if(RS_SANITIZE_${_san_upper} AND _allow_${_san})
      list(APPEND _sanitizers ${_flag_${_san}})
    endif()
  endforeach()

  # LSan: skip if ASan already active (redundant)
  if(RS_SANITIZE_LSAN AND _allow_lsan)
    if(RS_SANITIZE_ASAN AND _allow_asan)
      if(NOT _SAN_WARNED_LSAN_REDUNDANT)
        message(WARNING
          "[Sanitizers] LSan is redundant when ASan is active. Skipping LSan.")
        set(_SAN_WARNED_LSAN_REDUNDANT TRUE CACHE INTERNAL "")
      endif()
    else()
      list(APPEND _sanitizers leak)
    endif()
  endif()

  # --- Windows/Clang: locate ASan runtime DLL --------------------------------
  set(_asan_dll "")

  if(RS_SANITIZE_ASAN AND _allow_asan AND WIN32)
    execute_process(
      COMMAND ${CMAKE_CXX_COMPILER} --print-runtime-dir
      OUTPUT_VARIABLE _clang_rt_dir
      OUTPUT_STRIP_TRAILING_WHITESPACE
      RESULT_VARIABLE _clang_rt_result
    )

    if(NOT _clang_rt_result EQUAL 0 OR NOT _clang_rt_dir)
      message(WARNING
        "[Sanitizers] --print-runtime-dir failed. ASan disabled for '${target_name}'.")
      list(REMOVE_ITEM _sanitizers address)
    else()
      get_filename_component(_clang_rt_parent "${_clang_rt_dir}" DIRECTORY)
      set(_search_dirs
        "${_clang_rt_dir}"
        "${_clang_rt_parent}/x86_64-pc-windows-msvc"
        "${_clang_rt_parent}/x86_64-unknown-windows-msvc"
        "${_clang_rt_parent}/x86_64-unknown-windows-gnu"
        "${_clang_rt_parent}/x86_64-w64-windows-gnu"
      )

      foreach(_dir IN LISTS _search_dirs)
        file(GLOB _found "${_dir}/clang_rt.asan_dynamic-x86_64.dll")
        if(_found)
          list(GET _found 0 _asan_dll)
          break()
        endif()
      endforeach()

      if(NOT _asan_dll)
        message(WARNING
          "[Sanitizers] ASan runtime DLL not found. ASan disabled for '${target_name}'.\n"
          "Searched: ${_search_dirs}")
        list(REMOVE_ITEM _sanitizers address)
      endif()
    endif()
  endif()

  # --- Finalize --------------------------------------------------------------
  list(JOIN _sanitizers "," _san_list)

  if(NOT _san_list)
    return()
  endif()

  # --- Apply flags (PRIVATE, Debug + RelWithDebInfo only) --------------------
  set(_cfg "$<CONFIG:Debug,RelWithDebInfo>")

  target_compile_options(${target_name} PRIVATE
    "$<${_cfg}:-fsanitize=${_san_list}>"
    "$<${_cfg}:-fno-omit-frame-pointer>"
  )
  target_link_options(${target_name} PRIVATE
    "$<${_cfg}:-fsanitize=${_san_list}>"
  )

  message(STATUS "[Sanitizers] '${_san_list}' enabled for target '${target_name}'.")

  # --- Post-build: copy ASan DLL (Windows/Clang) -----------------------------
  if(_asan_dll)
    get_target_property(_already_copied ${_SAN_EXECUTABLE_TARGET} _SAN_ASAN_DLL_COPIED)
    if(NOT _already_copied)
      add_custom_command(TARGET ${_SAN_EXECUTABLE_TARGET} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
          "${_asan_dll}"
          "$<TARGET_FILE_DIR:${_SAN_EXECUTABLE_TARGET}>"
        COMMENT "[Sanitizers] Copying ASan runtime: ${_asan_dll}"
      )
      set_target_properties(${_SAN_EXECUTABLE_TARGET} PROPERTIES _SAN_ASAN_DLL_COPIED TRUE)
    endif()
  endif()

endfunction()
