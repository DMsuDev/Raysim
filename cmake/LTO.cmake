# ===========================================================================
# rs_enable_lto(<target>)
#
# Enables LTO for Release and RelWithDebInfo only. No-op when RS_ENABLE_LTO
# is OFF or sanitizers are active. Skipped on MinGW.
#
# Variables:
#   RS_ENABLE_LTO       BOOL  – master switch.
#   RS_LTO_PREFER_THIN  BOOL  – prefer ThinLTO on Clang (default: ON).
#
# Output:
#   RS_LTO_ENABLED_FOR_<target>  BOOL – TRUE if LTO was applied.
#
# Toolchain policy:
#   Clang       -> -flto=thin / -flto=full  (via RS_LTO_PREFER_THIN)
#   GCC         -> -flto
#   MSVC / rest -> CMake IPO (INTERPROCEDURAL_OPTIMIZATION)
# ===========================================================================

include_guard()

# ---------------------------------------------------------------------------
# Default for RS_LTO_PREFER_THIN
# ---------------------------------------------------------------------------
if(NOT DEFINED RS_LTO_PREFER_THIN)
  set(RS_LTO_PREFER_THIN ON)
endif()

# Shared genex for Release-like configs; used by Clang and GCC paths.
set(_RS_LTO_RELEASE_GENEX "$<CONFIG:Release,RelWithDebInfo>")

# Emit a warning once per configure run (avoids log spam in multi-target projects).
macro(_lto_warn_once cache_key)
  if(NOT _RS_LTO_WARNED_${cache_key})
    message(WARNING ${ARGN})
    set(_RS_LTO_WARNED_${cache_key} TRUE CACHE INTERNAL "")
  endif()
endmacro()

# ===========================================================================

function(rs_enable_lto target_name)

  if(NOT TARGET "${target_name}")
    message(FATAL_ERROR "[rs] LTO: target '${target_name}' does not exist.")
  endif()

  if(NOT RS_ENABLE_LTO)
    return()
  endif()

  if(RS_ENABLE_SANITIZERS)
    message(STATUS
      "[rs] LTO: disabled for '${target_name}' — sanitizers are active "
      "(LTO + sanitizers is unreliable).")
    return()
  endif()

  if(MINGW)
    _lto_warn_once(MINGW "[rs] LTO: MinGW has no reliable LTO support. Skipping.")
    return()
  endif()

  # -------------------------------------------------------------------------
  # Toolchain detection
  # -------------------------------------------------------------------------
  set(_is_gcc      FALSE)
  set(_is_clang    FALSE)
  set(_is_clang_cl FALSE)

  if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(_is_gcc TRUE)
  elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(_is_clang TRUE)
    if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
      set(_is_clang_cl TRUE)  # clang-cl falls through to IPO fallback
    endif()
  endif()

  # -------------------------------------------------------------------------
  # Clang — ThinLTO preferred for faster incremental links; opt out via
  # RS_LTO_PREFER_THIN=OFF to get Full LTO.
  # -------------------------------------------------------------------------
  if(_is_clang AND NOT _is_clang_cl)

    if(RS_LTO_PREFER_THIN)
      set(_lto_flag  "-flto=thin")
      set(_lto_label "ThinLTO")
    else()
      set(_lto_flag  "-flto=full")
      set(_lto_label "Full LTO")
    endif()

    target_compile_options("${target_name}" PRIVATE
      $<${_RS_LTO_RELEASE_GENEX}:${_lto_flag}>
    )
    target_link_options("${target_name}" PRIVATE
      $<${_RS_LTO_RELEASE_GENEX}:${_lto_flag}>
    )

    message(STATUS
      "[rs] LTO: ${_lto_label} enabled for '${target_name}' (Release / RelWithDebInfo).")

    set("RS_LTO_ENABLED_FOR_${target_name}" TRUE PARENT_SCOPE)
    return()

  endif()

  # -------------------------------------------------------------------------
  # GCC — plain -flto; parallelism is delegated to the build system.
  # -------------------------------------------------------------------------
  if(_is_gcc)

    target_compile_options("${target_name}" PRIVATE
      $<${_RS_LTO_RELEASE_GENEX}:-flto>
    )
    target_link_options("${target_name}" PRIVATE
      $<${_RS_LTO_RELEASE_GENEX}:-flto>
    )

    message(STATUS
      "[rs] LTO: Full LTO (-flto) enabled for '${target_name}' (Release / RelWithDebInfo).")

    set("RS_LTO_ENABLED_FOR_${target_name}" TRUE PARENT_SCOPE)
    return()

  endif()

  # -------------------------------------------------------------------------
  # Fallback — CMake IPO covers MSVC (/GL+/LTCG), clang-cl, Apple ld, etc.
  # IPO properties do not accept generator expressions, so we guard against
  # Debug builds explicitly for single-config generators.
  # -------------------------------------------------------------------------

  # Single-config: skip non-optimized builds early.
  if(NOT CMAKE_CONFIGURATION_TYPES)
    if(NOT CMAKE_BUILD_TYPE MATCHES "^(Release|RelWithDebInfo)$")
      message(STATUS
        "[rs] LTO: skipped for '${target_name}' "
        "(build type '${CMAKE_BUILD_TYPE}' is not Release or RelWithDebInfo).")
      set("RS_LTO_ENABLED_FOR_${target_name}" FALSE PARENT_SCOPE)
      return()
    endif()
  endif()

  # Cache the IPO check so it runs once per configure, not once per target.
  if(NOT DEFINED _RS_LTO_IPO_CHECKED)
    include(CheckIPOSupported)
    check_ipo_supported(RESULT _ipo_ok OUTPUT _ipo_out LANGUAGES CXX)
    set(_RS_LTO_IPO_CHECKED TRUE  CACHE INTERNAL "")
    set(_RS_LTO_IPO_SUPPORTED "${_ipo_ok}"  CACHE INTERNAL "")
    set(_RS_LTO_IPO_OUTPUT    "${_ipo_out}" CACHE INTERNAL "")
  endif()

  if(_RS_LTO_IPO_SUPPORTED)

    set_property(TARGET "${target_name}"
      PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE
    )

    # Multi-config: disable IPO for Debug and MinSizeRel to avoid overhead.
    if(CMAKE_CONFIGURATION_TYPES)
      set_property(TARGET "${target_name}"
        PROPERTY INTERPROCEDURAL_OPTIMIZATION_DEBUG FALSE
      )
      set_property(TARGET "${target_name}"
        PROPERTY INTERPROCEDURAL_OPTIMIZATION_MINSIZEREL FALSE
      )
    endif()

    message(STATUS "[rs] LTO: CMake IPO enabled for '${target_name}'.")
    set("RS_LTO_ENABLED_FOR_${target_name}" TRUE PARENT_SCOPE)

  else()
    _lto_warn_once(IPO_UNSUPPORTED
      "[rs] LTO: IPO not supported by this toolchain — skipping.\n"
      "Details: ${_RS_LTO_IPO_OUTPUT}"
    )
    set("RS_LTO_ENABLED_FOR_${target_name}" FALSE PARENT_SCOPE)
  endif()

endfunction()
