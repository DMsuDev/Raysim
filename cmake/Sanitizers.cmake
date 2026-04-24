# ================================================
# C++ Sanitizers configuration
# https://github.com/lefticus/cppbestpractices/blob/master/02-Use_the_Tools_Available.md
# ================================================

include_guard()

function(rs_enable_sanitizers target_name)
  if(NOT TARGET ${target_name})
    message(FATAL_ERROR "Target '${target_name}' does not exist")
  endif()

  if(NOT RS_ENABLE_SANITIZERS)
    return()
  endif()

  # Warn if sanitizers are enabled in a non-Debug build
  if(NOT CMAKE_CONFIGURATION_TYPES AND
     NOT CMAKE_BUILD_TYPE MATCHES "Debug|RelWithDebInfo")
    message(WARNING "Sanitizers are usually used with Debug builds")
  endif()

  # Compiler-specific flags
  if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")

    # Sanitizers are not reliably supported under MinGW on Windows
    if(MINGW)
      message(WARNING "Sanitizers are not supported with MinGW -> skipping")
      return()
    endif()

    set(SANITIZERS)

    # Compatibility checks
    if(RS_ENABLE_TSAN AND (RS_ENABLE_ASAN OR RS_ENABLE_MSAN))
      message(FATAL_ERROR "TSan incompatible with ASan/MSan")
    endif()

    # Determine which sanitizers to enable
    if(RS_ENABLE_ASAN)
      list(APPEND SANITIZERS address)
    endif()

    if(RS_ENABLE_UBSAN)
      list(APPEND SANITIZERS undefined)
    endif()

    if(RS_ENABLE_TSAN)
      list(APPEND SANITIZERS thread)
    endif()

    if(RS_ENABLE_MSAN)
      if(NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        message(FATAL_ERROR "MemorySanitizer (MSan) requires Clang")
      endif()
      list(APPEND SANITIZERS memory)
    endif()

    # LeakSanitizer (only if ASan not enabled)
    if(RS_ENABLE_LSAN)
      if(RS_ENABLE_ASAN)
        message(WARNING "ASan includes leak detection -> skipping LSan")
      else()
        list(APPEND SANITIZERS leak)
      endif()
    endif()

    if(SANITIZERS)
      list(JOIN SANITIZERS "," SANITIZER_LIST)

      target_compile_options(${target_name} PRIVATE
        $<$<CONFIG:Debug,RelWithDebInfo>:
          -fsanitize=${SANITIZER_LIST}
          -fno-omit-frame-pointer
          -fno-optimize-sibling-calls
          -fno-common
        >
      )
      target_link_options(${target_name} PRIVATE
        $<$<CONFIG:Debug,RelWithDebInfo>:
          -fsanitize=${SANITIZER_LIST}
        >
      )
    endif()

  elseif(MSVC)

    # MSVC only supports AddressSanitizer
    if(RS_ENABLE_ASAN)
      target_compile_options(${target_name} PRIVATE
      $<$<CONFIG:Debug,RelWithDebInfo>:/fsanitize=address>)
    endif()

  endif()

endfunction()
